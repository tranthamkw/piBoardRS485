#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "getOmega.h"

static PyObject *cOmegaTemp(PyObject *self, PyObject *args) {
    int ch;
	float result;
    if (!PyArg_ParseTuple(args, "i", &ch))
        return NULL;
	result=omegaTemperature(ch);
	return PyFloat_FromDouble(result);
}


static PyMethodDef OmegaMethods[] = {
    { "get_Omega", cOmegaTemp, METH_VARARGS, "get Omega Temperature." },
    { NULL, NULL, 0, NULL }
};


static struct PyModuleDef omegamodule = {
    PyModuleDef_HEAD_INIT, "getOmega", NULL, -1, OmegaMethods
};

PyMODINIT_FUNC PyInit_Omega_Module(void) {
    return PyModule_Create2(&omegamodule,PYTHON_ABI_VERSION);
}
