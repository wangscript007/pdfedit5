// vim:tabstop=4:shiftwidth=4:noexpandtab:textwidth=80
/*
 * =====================================================================================
 *        Filename:  iproperty.h
 *     Description:  IProperty class 
 *         Created:  01/19/2006 11:23:44 PM CET
 *          Author:  jmisutka ()
 * =====================================================================================
 */

#ifndef _IPROPERTY_H_
#define _IPROPERTY_H_

// basic includes
#include "static.h"

// property modes
#include "modecontroller.h"


//=====================================================================================
namespace pdfobjects {
//=====================================================================================

//
// Forward declarations
// 
class CPdf;
class IProperty;
typedef observer::IObserverHandler<IProperty> IPropertyObserver;


/** Enum describing the type of a property. */
enum PropertyType 
{
		// Simple
		pNull 	= objNull, 
		pBool 	= objBool, 
		pInt 	= objInt, 
		pReal 	= objReal, 
		pString = objString,
		pName	= objName,
		pRef	= objRef,
		
		// Complex
		pArray	= objArray, 
		pDict	= objDict, 
		pStream	= objStream, 

		// Debug
		pOther	= objCmd,
		pOther1 = objError,
		pOther2 = objEOF,
		pOther3 = objNone
};

//
// String representation of object type
//
template<int i> inline
std::string getStringType () {return "Unknown";}
template<> inline
std::string getStringType<0> () {return "pBool";}
template<> inline
std::string getStringType<1> () {return "pInt";}
template<> inline
std::string getStringType<2> () {return "pReal";}
template<> inline
std::string getStringType<3> () {return "pString";}
template<> inline
std::string getStringType<4> () {return "pName";}
template<> inline
std::string getStringType<5> () {return "pNull";}
template<> inline
std::string getStringType<9> () {return "pRef";}
template<> inline
std::string getStringType<6> () {return "pArray";}
template<> inline
std::string getStringType<7> () {return "pDict";}
template<> inline
std::string getStringType<8> () {return "pStream";}

/** 
 * Prints property type.
 *  
 * Prints given type in human readable from instead of just number.
 * Uses getStringType method to get string representation.
 *
 * @param out String where to print.
 * @param type Type to print.
 * @return Reference to given string.
 */
std::string& operator<< (std::string& out, PropertyType type);

/** Object id number. */
typedef unsigned int ObjNum;
/** Object generation number. */
typedef unsigned int GenNum;

/** Values specifying indirect reference to an (x)pdf object. */
typedef struct IndiRef
{
	ObjNum	num; /**< Object's pdf identification number */
	GenNum	gen; /**< Object's pdf generation number */

	/** 
	 * Empty constructor.
	 * Initializes num and gen to invalid reference.
	 */
	IndiRef() : num(0), gen(0) {}
	
	/** 
	 * Initialize constructor.
	 * Sets num and gen according given reference.
	 * 
	 * @param ref Indirect Reference.
	 */
	IndiRef(const IndiRef& ref) : num(ref.num), gen(ref.gen)	{}

	/** 
	 * Initialize constructor.
	 * Sets num and gen according given reference.
	 * 
	 * @param ref Xpdf reference.
	 */
	IndiRef(const ::Ref& ref) : num(ref.num), gen(ref.gen) {}

	/** 
	 * Initialize constructor.
	 * Sets num and gen according given parameters.
	 * 
	 * @param _num Object number.
	 * @param _gen Generation number.
	 */
	IndiRef(int _num, int _gen) : num(_num), gen(_gen) {}

	IndiRef& operator= (const IndiRef& _r) { num = _r.num; gen = _r.gen; return *this;};
	bool operator== (const IndiRef& _r) const { return (num == _r.num && gen == _r.gen) ? true : false;};
			
} IndiRef;


/** 
 * Prints reference.
 * Prints given reference in ref[num, gen] format.
 * 
 * @param out String where to print.
 * @param ref Reference to print.
 * @return reference to given string.
 */
std::ostream & operator << (std::ostream & out, const IndiRef & ref);

/** 
 * Prints reference.
 * Prints given xpdf reference in ref[num, gen] format.
 * 
 * @param out String where to print.
 * @param ref Reference to print.
 * @return reference to given string.
 */
std::ostream & operator << (std::ostream & out, const ::Ref & ref);


/** 
 * Narrow interface describing properties of every pdf object. We use this 
 * interface when we want to access properties of pdf object.
 *
 * Each IProperty is associated with one pdf object. The object represents current state.
 * However, these changes are not visible by the (x)pdf till they are registered
 * in CXref with dispatchChange() method.
 *
 * When accessing complex properties, we have to know the type with which type we
 * are working. According to the type, we can cast this object to CObjectComplex<type> 
 * to get more functionality.
 *
 * REMARK: The connection to CPdf is stored in CPdf* and not smart pointer. This has a good reason
 * namely cyclic references of smart pointers.
 */
class IProperty : public IPropertyObserver
{
private:
	IndiRef 		ref;		/**< Object's pdf id and generation number. */
	PropertyMode	mode;		/**< Mode of this property. */
	CPdf* 			pdf;		/**< This object belongs to this pdf. */	
	bool			wantDispatch;/**< If true, changes are dispatched. */

	//
	// Constructors
	//
private:
	/** Copy constructor. */
	IProperty (const IProperty&) : IPropertyObserver() {};

protected:	

	/** Basic constructor. */
	IProperty (CPdf* _pdf = NULL);

	/** Constructor. */
	IProperty (CPdf* _pdf, const IndiRef& rf);


	//
	// Deep Copy
	//
public:
  
	/**
	 * Copy constructor. Returns deep copy.
	 *
	 * REMARK: This is an example of Template method design pattern.
	 * 
	 * @return Deep copy of this object.
	 */
	boost::shared_ptr<IProperty> clone () const;

protected:

	/**
	 * Implementation of clone method
 	 *
 	 * @return Deep copy of this object.
	 */
	virtual IProperty* doClone () const = 0;

	//
	// Pdf
	//
public:
  
	/**
	 * Set member variable pdf.
	 * <exception cref="ObjInvalidOperation"> Thrown when we want to set pdf association to 
	 * already associated object.
	 *
	 * @param p pdf that this object belongs to
	 */
	virtual void setPdf (CPdf* p);

	/**
	 * Returns pdf in which this object lives.
	 *
	 * @return Pdf that this object is associated with.
	 */
	CPdf* getPdf () const {return pdf;};

	//
	// IndiRef
	//
public:
	/**
	 * Returns object's identification number. If it is an inline object
	 * returns id of parent object.
	 *
	 * @return Indirect identification number and generation number.
	 */
	const IndiRef& getIndiRef () const {return ref;};


	/**
	 * Set object identification number and generation number.
	 *
	 * @param _r Indirect reference id and generation number.
	 */
	virtual void setIndiRef (const IndiRef& rf) {ref = rf;};

 
	/**
	 * Set object identification number and generation number.
	 *
	 * @param n Object's id.
	 * @param g Object's generation number.
	 */
	void setIndiRef (ObjNum n, GenNum g) {ref.num = n; ref.gen = g;};

	//
	// PropertyMode
	//	
public:

	/**
	 * Get mode of this property.
	 *
	 * @return Mode.
	 */
	PropertyMode getMode () const {return mode;};

	/**
	 * Set mode of a property.
	 *
	 * @param md Mode.
	 */
	void setMode (PropertyMode md) {mode = md;};
	

public:
  
    /** 
     * Returns pointer to derived object. 
     *
     * @return Object casted to desired type.
     */
    template<typename T>
    inline static
	boost::shared_ptr<T> getSmartCObjectPtr (const boost::shared_ptr<IProperty>& ptr) 
    {
    	STATIC_CHECK(sizeof(T)>=sizeof(IProperty),DESTINATION_TYPE_TOO_NARROW); 
		boost::shared_ptr<T> newptr = boost::dynamic_pointer_cast<T, IProperty> (ptr);
		if (newptr)
		{
	  	  	return newptr;
		}
		else
		{
			assert (!"doClone INCORRECTLY overriden!!" );
			throw CObjInvalidObject (); 
		}
    }

	/** 
     * Returns type of object. 
     *
     * @return Type of this class.
     */
    virtual PropertyType getType () const = 0;

	/**
	 * Returns string representation of actual object.
	 * 
	 * If it is an indirect object, we have to notify CXref.
	 */
	virtual void getStringRepresentation (std::string& str) const = 0;

	//
	// Dispatch change
	//
public:
	/**
 	 * Notify Writer object that this object has changed. 
	 *
	 * We have to call this function to make changes visible.
	 *
	 * REMARK: This is an example of Template method design pattern.
	 */
	void dispatchChange () const;

	/**
	 * Lock changes. Do not dispatch changes.
	 */
	void lockChange () {assert (true == wantDispatch); wantDispatch = false;}

	/**
	 * Unlock changes. Do dispatch changes.
	 */
	void unlockChange () {assert (false == wantDispatch); wantDispatch = true;}

	/**
	 * Create xpdf object.
	 *
	 * @return Xpdf object(s).
	 */
	virtual Object* _makeXpdfObject () const = 0;

	/**
	 * Destructor.
	 */
	virtual ~IProperty () {};

}; /* class IProperty */


//
// Helper functions
//

/** 
 * Checks whether pdf is valid instance.
 * 
 * @param pdf Pdf isntance to check.
 * @return true if pdf is not NULL, false otherwise.
 */
inline bool isPdfValid(CPdf* pdf)
	{ return (NULL !=pdf); }

/** 
 * Checks whether ip's pdf is valid.
 * 
 * @param ip Property to check.
 * @return isPdfValid(ip-&getPdf()).
 */
inline bool hasValidPdf(const IProperty& ip)
	{ return isPdfValid(ip.getPdf()); }
template<typename T> inline bool hasValidPdf(T ip)
	{ return isPdfValid(ip->getPdf()); }


/** 
 * Checks whether given reference is valid.
 * Reference is valid, if it is non NULL and object number is greater than 0.
 * 
 * @param ref Reference to check.
 * @return true if reference is valid, false otherwise.
 */
inline bool isRefValid(const IndiRef* ref)
	{ return (ref) && (ref->num>0); }

template<typename T> inline bool hasValidRef (T ip) 
	{ return isRefValid(&ip->getIndiRef()); }

inline bool hasValidRef (IProperty& ip) 
	{ return isRefValid(& ip.getIndiRef()); }


//
template<PropertyType Type>
inline bool isIPType (const IProperty& ip) {return (Type == ip.getType());}
template<PropertyType Type>
inline bool isIPType (boost::shared_ptr<IProperty> ip) {return (Type == ip->getType());}
/** Is IProperty of specified type. */
template<typename T> inline bool isNull	 (T& ip) {return isIPType<pNull> (ip);}
template<typename T> inline bool isInt 	 (T& ip) {return isIPType<pInt> (ip);}
template<typename T> inline bool isReal  (T& ip) {return isIPType<pReal> (ip);}
template<typename T> inline bool isString(T& ip) {return isIPType<pString> (ip);}
template<typename T> inline bool isName  (T& ip) {return isIPType<pName> (ip);}
template<typename T> inline bool isRef 	 (T& ip) {return isIPType<pRef> (ip);}
template<typename T> inline bool isDict  (T& ip) {return isIPType<pDict> (ip);}
template<typename T> inline bool isArray (T& ip) {return isIPType<pArray> (ip);}
template<typename T> inline bool isStream(T& ip) {return isIPType<pStream> (ip);}
	

// =====================================================================================
} // namespace pdfobjects
// =====================================================================================


#endif  //_IPROPERTY_H
