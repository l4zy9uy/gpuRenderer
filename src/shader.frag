#version 460 core
in vec2 texCoord;
in vec4 Position;
//--------------------------------------------------------------------------------------
uniform vec2 iMouse;
uniform vec2 iResolution;
uniform float iTime;
//--------------------------------------------------------------------------------------
out vec4 fragColor;
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
vec2 fragCoord = gl_FragCoord.xy;
struct Material {
    int type;
    vec3 albedo;
};
struct Ray {
    vec3 origin;
    vec3 direction;
};
struct Hit_record {
    vec3 p;
    vec3 normal;
    Material mat;
    float t;
    bool front_face;
};
struct Sphere {
    vec3 center;
    float radius;
    Material mat;
};
struct Interval {
    float min;
    float max;
};
struct Camera {
    float aspect_ratio;
    int width;
    vec3 center;
    int height;
    vec3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    float pixel_sample_scale;
    int sample_per_pixel;
};
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
const float PI = 3.1415926535897932384626433832795;
const float PI_2 = 1.57079632679489661923;
const float PI_4 = 0.785398163397448309616;
const float INFINITY = 2147483647.0;
const int RAND_MAX = 2147483647;
const int MAX_OBJECT = 16;
Sphere objects[MAX_OBJECT];
const Interval Interval_empty = Interval(+INFINITY, -INFINITY);
const Interval Interval_universe = Interval(-INFINITY, +INFINITY);
const int Lambertian = 0;
const int Metal = 1;
int current_num_object = 0;
float g_seed = 0.;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
vec3 at(in Ray ray, in float t);
vec3 ray_color(in Ray r, in int num_sphere);
bool hit_sphere(in Sphere sphere, in Ray r, in Interval ray_t, inout Hit_record rec);
void set_face_normal(inout Hit_record rec, in Ray r, in vec3 outward_normal);
bool hit_world(in Ray r, in Interval ray_t, inout Hit_record rec, in int num_sphere);
float size(in Interval interval);
bool contain(in Interval interval, in float x);
bool surround(in Interval interval, in float x);
float clamp(in Interval interval, in float x);
void initialize_camera(inout Camera camera);
void render(inout Camera camera);
Ray get_ray(inout Camera camera);
vec3 random(in float min, in float max);
vec3 random_in_unit_sphere(inout float seed);
float linear_to_gamma(in float linear_component);
bool material_scatter(in Ray r_in, in Hit_record rec, inout vec3 attenuation, inout Ray scattered);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------

uint base_hash(uvec2 p) {
    p = 1103515245U*((p >> 1U)^(p.yx));
    uint h32 = 1103515245U*((p.x)^(p.y>>3U));
    return h32^(h32 >> 16);
}

float hash1(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=.1,seed+=.1)));
    return float(n)*(1.0/float(0xffffffffU));
}

vec2 hash2(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=.1,seed+=.1)));
    uvec2 rz = uvec2(n, n*48271U);
    return vec2(rz.xy & uvec2(0x7fffffffU))/float(0x7fffffff);
}

vec3 hash3(inout float seed) {
    uint n = base_hash(floatBitsToUint(vec2(seed+=.1,seed+=.1)));
    uvec3 rz = uvec3(n, n*16807U, n*48271U);
    return vec3(rz & uvec3(0x7fffffffU))/float(0x7fffffff);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
float degrees_to_radians(in float degrees) {
    return degrees * PI / 180.0;
}

float random_float() {
    // Returns a random real in [0,1).
    return hash1(g_seed) / (RAND_MAX);
}

float random_float(in float min, in float max) {
    // Returns a random real in [min,max).
    return float(min + (max-min)*random_float());
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
vec3 at(in Ray ray, in float t) {
    return ray.origin + t * ray.direction;
}

vec3 ray_color(in Ray r, in int num_sphere) {
    vec3 col = vec3(1);
    int depth = 4;

    for (int i = 0; i < depth; i++) {
        Hit_record rec;
        if (hit_world(r, Interval(0.01, INFINITY), rec, num_sphere)) {
            Ray scattered;
            vec3 attenuation;
            vec3 direction = normalize(rec.normal + random_in_unit_sphere(g_seed));
//            col *= 0.5;
//            r = Ray(rec.p, direction); // Update the ray for the next iteration
            if(material_scatter(r, rec, attenuation, scattered)) {
                col *= attenuation;
                r = scattered;
            }
            else return vec3(0);
        } else {
            float t = .5*r.direction.y + .5;
            col *= mix(vec3(1),vec3(.5,.7,1), t);
            return col;
        }
    }
    col = vec3(linear_to_gamma(col.r),
                linear_to_gamma(col.g),
                linear_to_gamma(col.b));
    return col;
}

bool hit_sphere(in Sphere sphere, in Ray r, in Interval ray_t, inout Hit_record rec) {
    vec3 oc = sphere.center - r.origin;
    float a = dot(r.direction, r.direction);
    float h = dot(r.direction, oc);
    float c = dot(oc, oc) - sphere.radius*sphere.radius;
    float discriminant = h*h - a*c;

    if (discriminant < 0) {
        return false;
    }

    float sqrtd = sqrt(discriminant);
    float root = (h - sqrtd) / a;
    if(!surround(ray_t, root)) {
        root = (h+sqrtd) / a;
        if(!surround(ray_t, root)) {
            return false;
        }
    }

    rec.t = root;
    rec.p = at(r, rec.t);
    vec3 outward_normal = (rec.p - sphere.center) / sphere.radius;
    set_face_normal(rec, r, outward_normal);
    rec.mat = sphere.mat;
    return true;
}

void set_face_normal(inout Hit_record rec, in Ray r, in vec3 outward_normal) {
    rec.front_face = dot(r.direction, outward_normal) < 0;
    rec.normal = rec.front_face ? outward_normal : -outward_normal;
}

bool hit_world(in Ray r, in Interval ray_t, inout Hit_record rec, in int num_sphere) {
    Hit_record temp_rec;
    bool hit_anything = false;
    float closet_so_far = ray_t.max;
    for(int i = 0; i < num_sphere; i++) {
        if(hit_sphere(objects[i], r, Interval(ray_t.min, closet_so_far), temp_rec)) {
            hit_anything = true;
            closet_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}
bool surround(in Interval interval, in float x) {
    return interval.min < x && x < interval.max;
}
void initialize_camera(inout Camera camera) {
    camera.aspect_ratio = iResolution.x / iResolution.y;
    camera.width = int(iResolution.x);
    int height = int(camera.width / camera.aspect_ratio);
    camera.height = (height < 1) ? 1 : height;
    camera.pixel_sample_scale = 1.0 / float(camera.sample_per_pixel);

    float focal_length = 1.0;
    float viewport_height = 2.0;
    float viewport_width = viewport_height * camera.width / camera.height;
    camera.center = vec3(0.0);

    vec3 viewport_u = vec3(viewport_width, 0.0, 0.0);
    vec3 viewport_v = vec3(0.0, viewport_height, 0.0);

    camera.pixel_delta_u = viewport_u / camera.width;
    camera.pixel_delta_v = viewport_v / camera.height;
    vec3 viewport_upper_left = camera.center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    camera.pixel00_loc = viewport_upper_left + 0.5 * (camera.pixel_delta_u + camera.pixel_delta_v);
}
void render(inout Camera camera) {
    initialize_camera(camera);
    vec3 pixel_color = vec3(0);
    for(int i_sample = 0; i_sample < camera.sample_per_pixel; i_sample++) {
        Ray r = get_ray(camera);
        pixel_color += ray_color(r, current_num_object);
    }
    fragColor = vec4(pixel_color * camera.pixel_sample_scale, 1.0);
}
Ray get_ray(inout Camera camera) {
    vec3 offset = hash3(g_seed);
    vec3 pixel_sample = camera.pixel00_loc + ((fragCoord.x + offset.x) * camera.pixel_delta_u) + ((fragCoord.y + offset.y) * camera.pixel_delta_v);
    vec3 ray_origin = camera.center;
    vec3 ray_direction = pixel_sample - ray_origin;
    return Ray(ray_origin, ray_direction);
}
vec3 random(in float min, in float max) {
    return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
}

vec3 random_in_unit_sphere(inout float seed) {
    vec3 h = hash3(seed) * vec3(2.,6.28318530718,1.)-vec3(1,0,0);
    float phi = h.y;
    float r = pow(h.z, 1./3.);
    return r * vec3(sqrt(1.-h.x*h.x)*vec2(sin(phi),cos(phi)),h.x);
}
float linear_to_gamma(float linear_component)
{
    if (linear_component > 0)
        return sqrt(linear_component);

    return 0;
}
bool material_scatter(in Ray r_in, in Hit_record rec, inout vec3 attenuation, inout Ray scattered) {
    if(rec.mat.type == Lambertian) {
        vec3 scatter_direction = rec.normal + random_in_unit_sphere(g_seed);
        scattered = Ray(rec.p, scatter_direction);
        attenuation = rec.mat.albedo;
        return true;
    }
    else if(rec.mat.type == Metal) {
        vec3 reflected = reflect(r_in.direction, rec.normal);
        scattered = Ray(rec.p, reflected);
        attenuation = rec.mat.albedo;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
void main() {
    Material mat_ground = Material(0, vec3(0.8f, 0.8f, 0.0f));
    Material mat_center = Material(0, vec3(0.1f, 0.2f, 0.5f));
    Material mat_left = Material(1, vec3(0.8f));
    Material mat_right = Material(1, vec3(0.8f, 0.6f, 0.2f));
    objects[0] = Sphere(vec3(0, -100.5, -1), 100, mat_ground);
    objects[1] = Sphere(vec3(0, 0, -1.2), 0.5, mat_center);
    objects[2] = Sphere(vec3(-1, 0, -1), 0.5, mat_left);
    objects[3] = Sphere(vec3(1, 0, -1), 0.5, mat_right);

    current_num_object = 4;
    Camera cam;
    cam.sample_per_pixel = 200;
    initialize_camera(cam);
    render(cam);
}