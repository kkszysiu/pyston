#ifndef Py_CEVAL_H
#define Py_CEVAL_H
#ifdef __cplusplus
extern "C" {
#endif

#define Py_EnterRecursiveCall(where)                                    \
            (false)
#define Py_LeaveRecursiveCall()                         \
            (true)


#ifdef __cplusplus
}
#endif
#endif /* !Py_CEVAL_H */
