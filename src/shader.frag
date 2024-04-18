#version 460 core
in vec2 texCoord;
in vec4 Position;
//--------------------------------------------------------------------------------------
uniform vec2 iMouse;
uniform vec2 iResolution;
uniform float iTime;
//--------------------------------------------------------------------------------------
out vec4 fragColor;
//--------------------------------------------------------------------------------------
const float PI = 3.1415926535897932384626433832795;
const float PI_2 = 1.57079632679489661923;
const float PI_4 = 0.785398163397448309616;
const float INFINITY = 2147483647.0;
const int MAX_OBJECT = 16;
//--------------------------------------------------------------------------------------
vec2 fragCoord = gl_FragCoord.xy;
struct Ray {
    vec3 origin;
    vec3 direction;
};
struct hit_record {
    vec3 p;
    vec3 normal;
    float t;
    bool front_face;
};
struct Sphere {
    vec3 center;
    float radius;
};
Sphere objects[MAX_OBJECT];
//--------------------------------------------------------------------------------------

vec3 at(in Ray ray, in float t);
vec3 ray_color(in Ray r, in int num_sphere);
bool hit_sphere(in Sphere sphere, in Ray r, in float ray_tmin, in float ray_tmax, inout hit_record rec);
void set_face_normal(inout hit_record rec, in Ray r, in vec3 outward_normal);
bool hit_world(in Ray r, in float ray_tmin, in float ray_tmax, inout hit_record rec, in int num_sphere);
//--------------------------------------------------------------------------------------

vec3 at(in Ray ray, in float t) {
    return ray.origin + t * ray.direction;
}
vec3 ray_color(in Ray r, in int num_sphere) {
    hit_record rec;
    if (hit_world(r, 0.0, INFINITY, rec, num_sphere)) {
        return 0.5 * (rec.normal + vec3(1,1,1));
    }

    vec3 unit_direction = normalize(r.direction);
    float a = 0.5*unit_direction.y + 1.0;
    return (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
}

bool hit_sphere(in Sphere sphere, in Ray r, in float ray_tmin, in float ray_tmax, inout hit_record rec) {
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
    if(root <= ray_tmin || root >= ray_tmax) {
        root = (h+sqrtd) / a;
        if(root <= ray_tmin || root >= ray_tmax) {
            return false;
        }
    }

    rec.t = root;
    rec.p = at(r, rec.t);
    vec3 outward_normal = (rec.p - sphere.center) / sphere.radius;
    set_face_normal(rec, r, outward_normal);

    return true;
}

void set_face_normal(inout hit_record rec, in Ray r, in vec3 outward_normal) {
    rec.front_face = dot(r.direction, outward_normal) < 0;
    rec.normal = rec.front_face ? outward_normal : -outward_normal;
}

bool hit_world(in Ray r, in float ray_tmin, in float ray_tmax, inout hit_record rec, in int num_sphere) {
    hit_record temp_rec;
    bool hit_anything = false;
    float closet_so_far = ray_tmax;
    for(int i = 0; i < num_sphere; i++) {
        if(hit_sphere(objects[i], r, ray_tmin, closet_so_far, temp_rec)) {
            hit_anything = true;
            closet_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

void main() {
    vec2 uv = (fragCoord / iResolution.xy);
    float aspect_ratio = iResolution.x / iResolution.y;
    int width = int(iResolution.x);
    int height = int(width / aspect_ratio);
    height = (height < 1) ? 1 : height;

    float focal_length = 1.0;
    float viewport_height = 2.0;
    float viewport_width = viewport_height * width / height;
    vec3 camera_center = vec3(0.0);

    vec3 viewport_u = vec3(viewport_width, 0.0, 0.0);
    vec3 viewport_v = vec3(0.0, viewport_height, 0.0);

    vec3 pixel_delta_u = viewport_u / width;
    vec3 pixel_delta_v = viewport_v / height;

    vec3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    vec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    vec3 pixel_center = pixel00_loc + (fragCoord.x * pixel_delta_u) + (fragCoord.y * pixel_delta_v);
    vec3 ray_direction = pixel_center - camera_center;
    Ray r = Ray(camera_center, ray_direction);

    objects[0] = Sphere(vec3(0, 0, -1), 0.5);
    objects[1] = Sphere(vec3(0, -100.5, -1), 100);
    vec3 pixel_color = ray_color(r, 2);
    fragColor = vec4(pixel_color, 1.0);
}