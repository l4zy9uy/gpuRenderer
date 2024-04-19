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
struct Ray {
    vec3 origin;
    vec3 direction;
};
struct Hit_record {
    vec3 p;
    vec3 normal;
    float t;
    bool front_face;
};
struct Sphere {
    vec3 center;
    float radius;
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
const Interval interval_empty = Interval(+INFINITY, -INFINITY);
const Interval interval_universe = Interval(-INFINITY, +INFINITY);
int current_num_object = 0;
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
vec3 sample_square();
Ray get_ray(inout Camera camera);
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if 1
//Modified from: iq's "Integer Hash - III" (https://www.shadertoy.com/view/4tXyWN)
uint baseHash(uvec3 p)
{
    p = 1103515245U*((p.xyz >> 1U)^(p.yzx));
    uint h32 = 1103515245U*((p.x^p.z)^(p.y>>3U));
    return h32^(h32 >> 16);
}

uint baseHash(uint p)
{
    p = 1103515245U*((p >> 1U)^(p));
    uint h32 = 1103515245U*((p)^(p>>3U));
    return h32^(h32 >> 16);
}
#else
//XXHash32 based (https://github.com/Cyan4973/xxHash)
uint baseHash(uvec3 p)
{
    const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
    const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
    uint h32 =  p.z + PRIME32_5 + p.x*PRIME32_3;
    h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17)));
    h32 += p.y * PRIME32_3;
    h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17))); //Initial testing suggests this line could be omitted for extra perf
    h32 = PRIME32_2*(h32^(h32 >> 15));
    h32 = PRIME32_3*(h32^(h32 >> 13));
    return h32^(h32 >> 16);
}

uint baseHash(uint p)
{
    const uint PRIME32_2 = 2246822519U, PRIME32_3 = 3266489917U;
    const uint PRIME32_4 = 668265263U, PRIME32_5 = 374761393U;
    uint h32 = p + PRIME32_5;
    h32 = PRIME32_4*((h32 << 17) | (h32 >> (32 - 17))); //Initial testing suggests this line could be omitted for extra perf
    h32 = PRIME32_2*(h32^(h32 >> 15));
    h32 = PRIME32_3*(h32^(h32 >> 13));
    return h32^(h32 >> 16);
}
#endif

//---------------------3D input---------------------
float hash13(uvec3 x)
{
    uint n = baseHash(x);
    return float(n)*(1.0/float(0xffffffffU));
}

vec2 hash23(uvec3 x)
{
    uint n = baseHash(x);
    uvec2 rz = uvec2(n, n*48271U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec2((rz.xy >> 1) & uvec2(0x7fffffffU))/float(0x7fffffff);
}

vec3 hash33(uvec3 x)
{
    uint n = baseHash(x);
    uvec3 rz = uvec3(n, n*16807U, n*48271U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec3((rz >> 1) & uvec3(0x7fffffffU))/float(0x7fffffff);
}

vec4 hash43(uvec3 x)
{
    uint n = baseHash(x);
    uvec4 rz = uvec4(n, n*16807U, n*48271U, n*69621U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec4((rz >> 1) & uvec4(0x7fffffffU))/float(0x7fffffff);
}

//---------------------1D input---------------------
float hash11(uint x)
{
    uint n = baseHash(x);
    return float(n)*(1.0/float(0xffffffffU));
}

vec2 hash21(uint x)
{
    uint n = baseHash(x);
    uvec2 rz = uvec2(n, n*48271U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec2((rz.xy >> 1) & uvec2(0x7fffffffU))/float(0x7fffffff);
}

vec3 hash31(uint x)
{
    uint n = baseHash(x);
    uvec3 rz = uvec3(n, n*16807U, n*48271U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec3((rz >> 1) & uvec3(0x7fffffffU))/float(0x7fffffff);
}

vec4 hash41(uint x)
{
    uint n = baseHash(x);
    uvec4 rz = uvec4(n, n*16807U, n*48271U, n*69621U); //see: http://random.mat.sbg.ac.at/results/karl/server/node4.html
    return vec4((rz >> 1) & uvec4(0x7fffffffU))/float(0x7fffffff);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
float degrees_to_radians(in float degrees) {
    return degrees * PI / 180.0;
}

float random_float() {
    // Returns a random real in [0,1).
    return hash11(int(fragCoord.x)) / (RAND_MAX);
}

float random_float(in float min, in float max) {
    // Returns a random real in [min,max).
    return float(min + (max-min)*random_float());
}
vec3 sample_square() {
    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    return vec3(random_float() - 0.5, random_float() - 0.5, 0);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
vec3 at(in Ray ray, in float t) {
    return ray.origin + t * ray.direction;
}

vec3 ray_color(in Ray r, in int num_sphere) {
    Hit_record rec;
    if (hit_world(r, Interval(0.0, INFINITY), rec, num_sphere)) {
        return 0.5 * (rec.normal + vec3(1,1,1));
    }

    vec3 unit_direction = normalize(r.direction);
    float a = 0.5*unit_direction.y + 1.0;
    return (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
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

float size(in Interval interval) {
    return interval.max - interval.min;
}
bool contain(in Interval interval, in float x) {
    return interval.min <= x && x <= interval.max;
}
bool surround(in Interval interval, in float x) {
    return interval.min < x && x < interval.max;
}
float clamp(in Interval interval, in float x) {
    if (x < interval.min) return interval.min;
    if (x > interval.max) return interval.max;
    return x;
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
    vec3 offset = hash31(int(fragCoord.x*10));
    vec3 pixel_sample = camera.pixel00_loc + ((fragCoord.x + offset.x) * camera.pixel_delta_u) + ((fragCoord.y + offset.y) * camera.pixel_delta_v);
    vec3 ray_origin = camera.center;
    vec3 ray_direction = pixel_sample - ray_origin;
    return Ray(ray_origin, ray_direction);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------
void main() {
    objects[0] = Sphere(vec3(0, 0, -1), 0.5);
    objects[1] = Sphere(vec3(0, -100.5, -1), 100);
    current_num_object = 2;
    Camera cam;
    cam.sample_per_pixel = 200;
    initialize_camera(cam);
    render(cam);
}