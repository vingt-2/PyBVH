#include <Python.h>
#include "SkeletalMotion.h"

struct module_state
{
	PyObject* error;
};

PyObject* Vector_ToPython(const float* data, int vectorSize) 
{
	PyObject* listObj = PyList_New(3);
	if (!listObj) throw logic_error("Unable to allocate memory for Python list");
	for (unsigned int i = 0; i < 3; i++) 
	{
		PyObject *num = PyFloat_FromDouble((double)data[i]);
		if (!num) 
		{
			Py_DECREF(listObj);
			throw logic_error("Unable to allocate memory for Python list");
		}
		PyList_SET_ITEM(listObj, i, num);
	}
	return listObj;
}

PyObject* VectorList_ToPython(const vector<float*> &data, int vectorSize)
{
	PyObject* listObj = PyList_New(data.size());
	if (!listObj) throw logic_error("Unable to allocate memory for Python list");
	for (unsigned int i = 0; i < data.size(); i++)
	{
		
		PyObject *list = Vector_ToPython(data[i], vectorSize);
		if (!list)
		{
			Py_DECREF(listObj);
			throw logic_error("Unable to allocate memory for Python list");
		}
		PyList_SET_ITEM(listObj, i, list);
	}
	return listObj;
}

PyObject* Vector3List_ToPython(const vector<vec3> &data)
{
	PyObject* listObj = PyList_New(data.size());
	if (!listObj) throw logic_error("Unable to allocate memory for Python list");
	for (unsigned int i = 0; i < data.size(); i++)
	{
		//PyObject* dict = PyDict_New(); <-- Dictionary test
		PyObject *list = Vector_ToPython(&(data[i].x), 3);
		if (!list)
		{
			Py_DECREF(listObj);
			throw logic_error("Unable to allocate memory for Python list");
		}
		PyList_SET_ITEM(listObj, i, list);
	}
	return listObj;
}

PyObject* mat4ToPython(const mat4 &data)
{
	PyObject* listObj = PyList_New(4);
	if (!listObj) throw logic_error("Unable to allocate memory for Python list");
	for (unsigned int i = 0; i < 4; i++)
	{
		PyObject *list = PyList_New(4);
		if (!list)
		{
			Py_DECREF(listObj);
			throw logic_error("Unable to allocate memory for Python list");
		}
		for (unsigned int j = 0; j < 4; j++)
		{
			PyList_SET_ITEM(list, j, PyFloat_FromDouble((double)data[i][j]));
		}
		PyList_SET_ITEM(listObj, i, list);
	}
	return listObj;
}

PyObject *cSkeletalMotion_LoadBVH(PyObject *self, PyObject *args) {
	char* sFilePath;
	if (!PyArg_ParseTuple(args, "s", &sFilePath))
		return NULL;

	SkeletalMotion* inst = SkeletalMotion::BVHImport(string(sFilePath));
	
	return PyCapsule_New(inst,"SkeletalMotion", NULL);
}

PyObject *cSkeletalMotion_GetName(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*) PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	return PyUnicode_FromString(skeletalMotionInstance->GetName().data());
}

PyObject *cSkeletalMotion_GetFrameCount(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*) PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	return PyLong_FromLong((long)skeletalMotionInstance->GetFrameCount());
}

PyObject* cSkeletalMotion_SetNormalizedScale(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	float scale;
	PyArg_ParseTuple(args, "Of", &pyInst, &scale);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	skeletalMotionInstance->SetNormalizedScaleWithMultiplier(scale);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject* cSkeletalMotion_GetJointPositions(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	int frameIndex;
	int addRoot;
	PyArg_ParseTuple(args, "Oii", &pyInst, &frameIndex, &addRoot);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	std::vector<vec3> jointPositions;
	skeletalMotionInstance->QuerySkeletalAnimation(frameIndex, 0, (bool)addRoot, &jointPositions, NULL, NULL, NULL);

	return Vector3List_ToPython(jointPositions);
}

PyObject* cSkeletalMotion_GetJointNames(PyObject *self, PyObject *args) {
	PyObject *pyInst;

	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	std::vector<std::string> jointNames;
	skeletalMotionInstance->GetRoot(0)->GetJointNamesInOrder(jointNames);

	PyObject* listObj = PyList_New(jointNames.size());
	if (!listObj) throw logic_error("Unable to allocate memory for Python list");
	for (unsigned int i = 0; i < jointNames.size(); i++)
	{
		PyObject *name = PyUnicode_FromString(jointNames.at(i).data());
		if (!name)
		{
			Py_DECREF(listObj);
			throw logic_error("Unable to allocate memory for Python list");
		}
		PyList_SET_ITEM(listObj, i, name);
	}
	return listObj;
}

PyObject* cSkeletalMotion_BonesByJointNames(PyObject *self, PyObject *args) {
	PyObject *pyInst;

	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	vector<pair<string, string>> bonesByName;
	skeletalMotionInstance->GetRoot(0)->QuerySkeleton(NULL, &bonesByName);

	PyObject* listObj = PyList_New(bonesByName.size());
	if (!listObj) throw logic_error("Unable to allocate memory for Python list");
	for (unsigned int i = 0; i < bonesByName.size(); i++)
	{
		PyObject* pairList = PyList_New(2);
		PyObject *firstJoint = PyUnicode_FromString(bonesByName[i].first.data());
		PyObject *secondJoint = PyUnicode_FromString(bonesByName[i].second.data());

		if (!firstJoint || !secondJoint || !pairList)
		{
			Py_DECREF(listObj);
			Py_DECREF(pairList);
			throw logic_error("Unable to allocate memory for Python list");
		}

		PyList_SET_ITEM(pairList, 0, firstJoint);
		PyList_SET_ITEM(pairList, 1, secondJoint);

		PyList_SET_ITEM(listObj, i, pairList);
	}
	return listObj;
}

PyObject* cSkeletalMotion_GetCumulativeTransform(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	int frameIndex;
	int addRoot;
	PyArg_ParseTuple(args, "Oii", &pyInst, &frameIndex, &addRoot);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	std::unordered_map<string,Transform> jointTransformsByName;
	skeletalMotionInstance->QuerySkeletalAnimation(frameIndex, 0, (bool)addRoot, NULL, NULL, NULL, &jointTransformsByName);

	PyObject* jointTransformPythonDict = PyDict_New();

	for (auto jointTransform : jointTransformsByName)
	{
		PyObject* mat4AsList = mat4ToPython(jointTransform.second.m_transformMatrix);

		PyObject* pyString = PyUnicode_FromString(jointTransform.first.data());

		PyDict_SetItem(jointTransformPythonDict, pyString, mat4AsList);
	}

	return jointTransformPythonDict;
}

PyObject* cSkeletalMotion_GetSamplingRate(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	return PyFloat_FromDouble((double)1.0f/skeletalMotionInstance->GetSamplingRate());
}

PyObject* cSkeletalMotion_GetNormalizedScale(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCapsule_GetPointer(pyInst, "SkeletalMotion");

	return PyFloat_FromDouble((double)skeletalMotionInstance->GetScale());
}

PyMethodDef cSkeletalMotion_funcs[] = {
	{ "loadBVH" , cSkeletalMotion_LoadBVH, METH_VARARGS, "PyBVH::LoadBVH" },
	{ "getName", cSkeletalMotion_GetName, METH_VARARGS, "SkeletalMotion::GetName" },
	{ "getFrameCount", cSkeletalMotion_GetFrameCount, METH_VARARGS, "SkeletalMotion::GetFrameCount" },
	{ "setNormalizedScale", cSkeletalMotion_SetNormalizedScale, METH_VARARGS, "SkeletalMotion::SetNormalizedScale" },
	{ "getJointPositions", cSkeletalMotion_GetJointPositions, METH_VARARGS, "SkeletalMotion::GetJointPositions" },
	{ "getCumTransform", cSkeletalMotion_GetCumulativeTransform, METH_VARARGS, "SkeletalMotion::GetCumulativeTransforms" },
	{ "getSamplingRate", cSkeletalMotion_GetSamplingRate, METH_VARARGS, "SkeletalMotion::GetSamplingRate" },
	{ "getJointNames", cSkeletalMotion_GetJointNames, METH_VARARGS, "SkeletalMotion::GetJointNames" },
	{ "getBonesByJointNames", cSkeletalMotion_BonesByJointNames, METH_VARARGS, "SkeletalMotion::GetBonesByJointNames" },
	{ "getNormalizedScale", cSkeletalMotion_GetNormalizedScale, METH_VARARGS, "SkeletalMotion::GetNormalizedScale" },
	{ NULL, NULL, 0, NULL }    /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct module_state _state;
#endif

#if PY_MAJOR_VERSION >= 3

static int myextension_traverse(PyObject *m, visitproc visit, void *arg) {
	Py_VISIT(GETSTATE(m)->error);
	return 0;
}

static int myextension_clear(PyObject *m) {
	Py_CLEAR(GETSTATE(m)->error);
	return 0;
}


static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	"cSkeletalMotion",
	NULL,
	sizeof(struct module_state),
	cSkeletalMotion_funcs,
	NULL,
	myextension_traverse,
	myextension_clear,
	NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC
PyInit_cSkeletalMotion(void)

#else
#define INITERROR return

void
initmyextension(void)
#endif
{
#if PY_MAJOR_VERSION >= 3
	PyObject *module = PyModule_Create(&moduledef);
#else
	PyObject *module = Py_InitModule("cSkeletalMotion", myextension_methods);
#endif

	if (module == NULL)
		INITERROR;
	struct module_state *st = GETSTATE(module);

	st->error = PyErr_NewException("PyBVH.Error", NULL, NULL);
	if (st->error == NULL) {
		Py_DECREF(module);
		INITERROR;
	}

#if PY_MAJOR_VERSION >= 3
	return module;
#endif
}

/* #include <Python.h>

#include "animation.h"
#include <cstdio>

PyObject* PySkeletalMotion;

PyObject *cSkeletalMotion_LoadBVH(PyObject *self, PyObject *args) {
	char* sFilePath;
	if (!PyArg_ParseTuple(args, "s", &sFilePath))
		return NULL;

	SkeletalMotion* inst = SkeletalMotion::BVHImport(string(sFilePath));

	return PyCObject_FromVoidPtr(inst, NULL);
}

PyObject *cSkeletalMotion_GetName(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*) PyCObject_AsVoidPtr(pyInst);

	return PyString_FromString(skeletalMotionInstance->GetName().data());
}

PyObject *cSkeletalMotion_GetFrameCount(PyObject *self, PyObject *args) {
	PyObject *pyInst;
	PyArg_ParseTuple(args, "O", &pyInst);
	SkeletalMotion* skeletalMotionInstance = (SkeletalMotion*)PyCObject_AsVoidPtr(pyInst);

	return PyInt_FromLong(skeletalMotionInstance->GetFrameCount());
}

PyMethodDef cSkeletalMotion_funcs[] = {
	{ "loadBVH" , cSkeletalMotion_LoadBVH, METH_VARARGS, "PyBVH::LoadBVH" },
	{ "getName", cSkeletalMotion_GetName, METH_VARARGS, "PyBVH::GetName" },
	{ "getFrameCount" , cSkeletalMotion_GetFrameCount, METH_VARARGS, "PyBVH::GetFrameCount" },
	{ NULL, NULL, 0, NULL }    /* Sentinel */ /*
};

PyMODINIT_FUNC initcSkeletalMotion() {
	Py_InitModule("cBVH", cSkeletalMotion_funcs);
	PyEval_InitThreads();
	PySkeletalMotion = PyObject_GetAttrString(PyImport_ImportModule("PyBVH.SkeletalMotion"), "SkeletalMotion");
	Py_INCREF(PySkeletalMotion);
} */