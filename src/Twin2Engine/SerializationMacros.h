#ifndef _SERIALIZATION_MACROS_H_
#define _SERIALIZATION_MACROS_H_


// Macro to clone each field
#define CloneField(fieldName) \
    cloned->fieldName = fieldName; \



// Macro to start the clone function definition
#define CloneFunctionStart(className, baseClassName) \
	protected: \
    className* Clone() const \
    { \
        className* cloned = new className(); \
        CloneTo(cloned); \
        return cloned; \
    } \
    void CloneTo(className* cloned) const \
    { \
        baseClassName::CloneTo(cloned);


// Macro to end the clone function definition
#define CloneFunctionEnd() \
    } \
    private:

// Macro to define the clone function
//#define CloneFunction(className, baseClassName, ...) \
//    CloneFunctionStart(className, baseClassName) \
//        CloneFields(__VA_ARGS__) \
//    CloneFunctionEnd()


#endif // !_SERIALIZATION_MACROS_H_