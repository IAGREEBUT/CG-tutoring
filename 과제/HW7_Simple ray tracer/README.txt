Debug folder : exe파일
1.raytracing folder : src파일

step1 ) 
	light 생성 (raytraceData.h 에 struct생성 ) -> makeLight함수 (raytrace.h)
	shader::shade함수 내에 diff/spec(blinn phong model이용)추가 
	
step2 ) halfspace추가 

	plane struct(raytraceData.h) -> makePlane함수 (raytrace.h)
	findPlaneNormal, rayPlaneIntersect함수 추가 하여 tracer::trace내에 사용 (tracer.h)

step3) vis(i) -> plane에 shadow적용안됨...
	rayColor함수 내에서 사용(raytrace.h) 
	구현은 vis(ray r)(tracer.h내에 구현)
