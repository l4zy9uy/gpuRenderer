#version 460 core
in vec2 texCoord;
in vec4 Position;

uniform vec2 iMouse;
uniform vec2 iResolution;
uniform float iTime;

out vec4 fragColor;
vec2 fragCoord = gl_FragCoord.xy;
struct Ray {
    vec3 origin;
    vec3 direction;
};
bool hit_sphere(in vec3 center, in float radius, in Ray r);
vec3 at(in Ray ray, in float t);
vec3 ray_color(in Ray r);
bool hit_sphere(in vec3 center, in float radius, in Ray r);

vec3 at(in Ray ray, in float t) {
    return ray.origin + t * ray.direction;
}
vec3 ray_color(in Ray r) {
    if(hit_sphere(vec3(0, 0, -1), 0.5, r)) {
        return vec3(1, 0, 0);
    }
    vec3 unit_direction = normalize(r.direction);
    float a = 0.5*unit_direction.y + 1.0;
    return (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
}

bool hit_sphere(in vec3 center, in float radius, in Ray r) {
    vec3 oc = center - r.origin;
    float a = dot(r.direction, r.direction);
    float b = -2.0 * dot(r.direction, oc);
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - 4*a*c;
    return (discriminant >= 0);
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
    vec3 viewport_v = vec3(0.0, -viewport_height, 0.0);

    vec3 pixel_delta_u = viewport_u / width;
    vec3 pixel_delta_v = viewport_v / height;

    vec3 viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    vec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    vec3 pixel_center = pixel00_loc + (fragCoord.x * pixel_delta_u) + (fragCoord.y * pixel_delta_v);
    vec3 ray_direction = pixel_center - camera_center;
    Ray r = Ray(camera_center, ray_direction);

    vec3 pixel_color = ray_color(r);
    fragColor = vec4(pixel_color, 1.0);
}