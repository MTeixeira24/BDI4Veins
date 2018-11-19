//
// Generated file, do not edit! Created by nedtool 5.4 from messages/regroupVoting/MemberExchange.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "MemberExchange_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(MemberExchange)

MemberExchange::MemberExchange(const char *name, short kind) : ::NegotiationMessage(name,kind)
{
    this->platoonId = 0;
    this->type = 0;
    idMembers_arraysize = 0;
    this->idMembers = 0;
    speedMembers_arraysize = 0;
    this->speedMembers = 0;
}

MemberExchange::MemberExchange(const MemberExchange& other) : ::NegotiationMessage(other)
{
    idMembers_arraysize = 0;
    this->idMembers = 0;
    speedMembers_arraysize = 0;
    this->speedMembers = 0;
    copy(other);
}

MemberExchange::~MemberExchange()
{
    delete [] this->idMembers;
    delete [] this->speedMembers;
}

MemberExchange& MemberExchange::operator=(const MemberExchange& other)
{
    if (this==&other) return *this;
    ::NegotiationMessage::operator=(other);
    copy(other);
    return *this;
}

void MemberExchange::copy(const MemberExchange& other)
{
    this->platoonId = other.platoonId;
    this->type = other.type;
    delete [] this->idMembers;
    this->idMembers = (other.idMembers_arraysize==0) ? nullptr : new int[other.idMembers_arraysize];
    idMembers_arraysize = other.idMembers_arraysize;
    for (unsigned int i=0; i<idMembers_arraysize; i++)
        this->idMembers[i] = other.idMembers[i];
    delete [] this->speedMembers;
    this->speedMembers = (other.speedMembers_arraysize==0) ? nullptr : new int[other.speedMembers_arraysize];
    speedMembers_arraysize = other.speedMembers_arraysize;
    for (unsigned int i=0; i<speedMembers_arraysize; i++)
        this->speedMembers[i] = other.speedMembers[i];
}

void MemberExchange::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::NegotiationMessage::parsimPack(b);
    doParsimPacking(b,this->platoonId);
    doParsimPacking(b,this->type);
    b->pack(idMembers_arraysize);
    doParsimArrayPacking(b,this->idMembers,idMembers_arraysize);
    b->pack(speedMembers_arraysize);
    doParsimArrayPacking(b,this->speedMembers,speedMembers_arraysize);
}

void MemberExchange::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::NegotiationMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->platoonId);
    doParsimUnpacking(b,this->type);
    delete [] this->idMembers;
    b->unpack(idMembers_arraysize);
    if (idMembers_arraysize==0) {
        this->idMembers = 0;
    } else {
        this->idMembers = new int[idMembers_arraysize];
        doParsimArrayUnpacking(b,this->idMembers,idMembers_arraysize);
    }
    delete [] this->speedMembers;
    b->unpack(speedMembers_arraysize);
    if (speedMembers_arraysize==0) {
        this->speedMembers = 0;
    } else {
        this->speedMembers = new int[speedMembers_arraysize];
        doParsimArrayUnpacking(b,this->speedMembers,speedMembers_arraysize);
    }
}

int MemberExchange::getPlatoonId() const
{
    return this->platoonId;
}

void MemberExchange::setPlatoonId(int platoonId)
{
    this->platoonId = platoonId;
}

int MemberExchange::getType() const
{
    return this->type;
}

void MemberExchange::setType(int type)
{
    this->type = type;
}

void MemberExchange::setIdMembersArraySize(unsigned int size)
{
    int *idMembers2 = (size==0) ? nullptr : new int[size];
    unsigned int sz = idMembers_arraysize < size ? idMembers_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        idMembers2[i] = this->idMembers[i];
    for (unsigned int i=sz; i<size; i++)
        idMembers2[i] = 0;
    idMembers_arraysize = size;
    delete [] this->idMembers;
    this->idMembers = idMembers2;
}

unsigned int MemberExchange::getIdMembersArraySize() const
{
    return idMembers_arraysize;
}

int MemberExchange::getIdMembers(unsigned int k) const
{
    if (k>=idMembers_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", idMembers_arraysize, k);
    return this->idMembers[k];
}

void MemberExchange::setIdMembers(unsigned int k, int idMembers)
{
    if (k>=idMembers_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", idMembers_arraysize, k);
    this->idMembers[k] = idMembers;
}

void MemberExchange::setSpeedMembersArraySize(unsigned int size)
{
    int *speedMembers2 = (size==0) ? nullptr : new int[size];
    unsigned int sz = speedMembers_arraysize < size ? speedMembers_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        speedMembers2[i] = this->speedMembers[i];
    for (unsigned int i=sz; i<size; i++)
        speedMembers2[i] = 0;
    speedMembers_arraysize = size;
    delete [] this->speedMembers;
    this->speedMembers = speedMembers2;
}

unsigned int MemberExchange::getSpeedMembersArraySize() const
{
    return speedMembers_arraysize;
}

int MemberExchange::getSpeedMembers(unsigned int k) const
{
    if (k>=speedMembers_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", speedMembers_arraysize, k);
    return this->speedMembers[k];
}

void MemberExchange::setSpeedMembers(unsigned int k, int speedMembers)
{
    if (k>=speedMembers_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", speedMembers_arraysize, k);
    this->speedMembers[k] = speedMembers;
}

class MemberExchangeDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MemberExchangeDescriptor();
    virtual ~MemberExchangeDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MemberExchangeDescriptor)

MemberExchangeDescriptor::MemberExchangeDescriptor() : omnetpp::cClassDescriptor("MemberExchange", "NegotiationMessage")
{
    propertynames = nullptr;
}

MemberExchangeDescriptor::~MemberExchangeDescriptor()
{
    delete[] propertynames;
}

bool MemberExchangeDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MemberExchange *>(obj)!=nullptr;
}

const char **MemberExchangeDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MemberExchangeDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MemberExchangeDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount() : 4;
}

unsigned int MemberExchangeDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *MemberExchangeDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "platoonId",
        "type",
        "idMembers",
        "speedMembers",
    };
    return (field>=0 && field<4) ? fieldNames[field] : nullptr;
}

int MemberExchangeDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='p' && strcmp(fieldName, "platoonId")==0) return base+0;
    if (fieldName[0]=='t' && strcmp(fieldName, "type")==0) return base+1;
    if (fieldName[0]=='i' && strcmp(fieldName, "idMembers")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "speedMembers")==0) return base+3;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MemberExchangeDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : nullptr;
}

const char **MemberExchangeDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MemberExchangeDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MemberExchangeDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MemberExchange *pp = (MemberExchange *)object; (void)pp;
    switch (field) {
        case 2: return pp->getIdMembersArraySize();
        case 3: return pp->getSpeedMembersArraySize();
        default: return 0;
    }
}

const char *MemberExchangeDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MemberExchange *pp = (MemberExchange *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MemberExchangeDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MemberExchange *pp = (MemberExchange *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getPlatoonId());
        case 1: return long2string(pp->getType());
        case 2: return long2string(pp->getIdMembers(i));
        case 3: return long2string(pp->getSpeedMembers(i));
        default: return "";
    }
}

bool MemberExchangeDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MemberExchange *pp = (MemberExchange *)object; (void)pp;
    switch (field) {
        case 0: pp->setPlatoonId(string2long(value)); return true;
        case 1: pp->setType(string2long(value)); return true;
        case 2: pp->setIdMembers(i,string2long(value)); return true;
        case 3: pp->setSpeedMembers(i,string2long(value)); return true;
        default: return false;
    }
}

const char *MemberExchangeDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *MemberExchangeDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MemberExchange *pp = (MemberExchange *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


