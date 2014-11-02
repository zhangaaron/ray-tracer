

class RayTracer{
	public:
		AggregatePrimitive* objList;
		std::vector<Light*>* lightList;
		Vector3f ambient;
		RayTracer(AggregatePrimitive* o, std::vector<Light*>* l, Vector3f a);
		void trace(Ray& ray, int depth, Vector3f* color);
	private:
};