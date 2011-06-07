#version 130

out vec4 FragColor;

uniform sampler3D volumeTex;
uniform sampler1D transferTex;
uniform mat4 modelView;
uniform mat4 modelViewInv;
uniform vec2 windowSize;
uniform vec3 rayOrigin;
uniform float transferScale;
uniform float transferOffset;
uniform float densityScale;

const float maxDist = sqrt(2.0);
const int numSamples = 128;
const float stepSize = maxDist / float(numSamples);

struct Ray {
	vec3 Origin;
	vec3 Dir;
};

struct AABB {
	vec3 Min;
	vec3 Max;
};

bool IntersectBox(Ray r, AABB aabb, out float t0, out float t1) {
	vec3 invR = 1.0 / r.Dir;
	vec3 tbot = invR * (aabb.Min-r.Origin);
	vec3 ttop = invR * (aabb.Max-r.Origin);
	vec3 tmin = min(ttop, tbot);
	vec3 tmax = max(ttop, tbot);
	vec2 t = max(tmin.xx, tmin.yz);
	t0 = max(t.x, t.y);
	t = min(tmax.xx, tmax.yz);
	t1 = min(t.x, t.y);
	return t0 <= t1;
}

void main() {

	vec3 rayDirection;
	rayDirection.xy = 2.0f * gl_FragCoord.xy / windowSize - 1.0f; // normalize to [-1, 1]
	rayDirection.z = -1.0f; // to je focalLenght. -1.0f je vredu vrednost :p, okoli 45 fov

	// extend rayDirection with 0 so we dont use translation when we mul with MW mat
	Ray eye = Ray(rayOrigin, normalize(vec4(rayDirection, 0.0) * modelViewInv).xyz);

	AABB aabb = AABB(vec3(-1.0), vec3(1.0));

	float tnear, tfar;

	IntersectBox(eye, aabb, tnear, tfar);
	if (tnear < 0.0) tnear = 0.0;

	vec3 rayStart = eye.Origin + eye.Dir * tnear;
	vec3 rayStop = eye.Origin + eye.Dir * tfar;

	rayStart = 0.5 * (rayStart + 1.0); // remap to [0, 1]
	rayStop = 0.5 * (rayStop + 1.0); // remap to [0, 1]

	vec3 pos = rayStart;
	vec3 step = normalize(rayStop - rayStart) * stepSize;
	float travel = distance(rayStop, rayStart);
	vec4 sum = vec4(0.0, 0.0, 0.0, 0.0); // color of ray

	for (int i = 0; i < numSamples && travel > 0.0; ++i, pos += step, travel -= stepSize) {

		float sample = texture(volumeTex, pos).x * densityScale;

		vec4 col = texture(transferTex, (sample - transferOffset) * transferScale);

		col.x *= col.w;
		col.y *= col.w;
		col.z *= col.w;

		if (sample <= 0.0) continue; // na zapadu nista novog

		if(sum.w > 0.95) break;

		sum += col * (1 - sum.w);



	}

	//FragColor.rgb = Lo;
	//FragColor.a = 1 - T;
	FragColor = sum;
}
