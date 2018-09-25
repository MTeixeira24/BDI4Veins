//
// Generated file, do not edit! Created by nedtool 5.4 from messages/voting/NotifyVote.msg.
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
#include "NotifyVote_m.h"

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

Register_Class(NotifyVote)

NotifyVote::NotifyVote(const char *name, short kind) : ::NegotiationMessage(name,kind)
{
    candidates_arraysize = 0;
    this->candidates = 0;
    contextArguments_arraysize = 0;
    this->contextArguments = 0;
    this->contextId = 0;
}

NotifyVote::NotifyVote(const NotifyVote& other) : ::NegotiationMessage(other)
{
    candidates_arraysize = 0;
    this->candidates = 0;
    contextArguments_arraysize = 0;
    this->contextArguments = 0;
    copy(other);
}

NotifyVote::~NotifyVote()
{
    delete [] this->candidates;
    delete [] this->contextArguments;
}

NotifyVote& NotifyVote::operator=(const NotifyVote& other)
{
    if (this==&other) return *this;
    ::NegotiationMessage::operator=(other);
    copy(other);
    return *this;
}

void NotifyVote::copy(const NotifyVote& other)
{
    delete [] this->candidates;
    this->candidates = (other.candidates_arraysize==0) ? nullptr : new int[other.candidates_arraysize];
    candidates_arraysize = other.candidates_arraysize;
    for (unsigned int i=0; i<candidates_arraysize; i++)
        this->candidates[i] = other.candidates[i];
    delete [] this->contextArguments;
    this->contextArguments = (other.contextArguments_arraysize==0) ? nullptr : new double[other.contextArguments_arraysize];
    contextArguments_arraysize = other.contextArguments_arraysize;
    for (unsigned int i=0; i<contextArguments_arraysize; i++)
        this->contextArguments[i] = other.contextArguments[i];
    this->contextId = other.contextId;
}

void NotifyVote::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::NegotiationMessage::parsimPack(b);
    b->pack(candidates_arraysize);
    doParsimArrayPacking(b,this->candidates,candidates_arraysize);
    b->pack(contextArguments_arraysize);
    doParsimArrayPacking(b,this->contextArguments,contextArguments_arraysize);
    doParsimPacking(b,this->contextId);
}

void NotifyVote::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::NegotiationMessage::parsimUnpack(b);
    delete [] this->candidates;
    b->unpack(candidates_arraysize);
    if (candidates_arraysize==0) {
        this->candidates = 0;
    } else {
        this->candidates = new int[candidates_arraysize];
        doParsimArrayUnpacking(b,this->candidates,candidates_arraysize);
    }
    delete [] this->contextArguments;
    b->unpack(contextArguments_arraysize);
    if (contextArguments_arraysize==0) {
        this->contextArguments = 0;
    } else {
        this->contextArguments = new double[contextArguments_arraysize];
        doParsimArrayUnpacking(b,this->contextArguments,contextArguments_arraysize);
    }
    doParsimUnpacking(b,this->contextId);
}

void NotifyVote::setCandidatesArraySize(unsigned int size)
{
    int *candidates2 = (size==0) ? nullptr : new int[size];
    unsigned int sz = candidates_arraysize < size ? candidates_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        candidates2[i] = this->candidates[i];
    for (unsigned int i=sz; i<size; i++)
        candidates2[i] = 0;
    candidates_arraysize = size;
    delete [] this->candidates;
    this->candidates = candidates2;
}

unsigned int NotifyVote::getCandidatesArraySize() const
{
    return candidates_arraysize;
}

int NotifyVote::getCandidates(unsigned int k) const
{
    if (k>=candidates_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", candidates_arraysize, k);
    return this->candidates[k];
}

void NotifyVote::setCandidates(unsigned int k, int candidates)
{
    if (k>=candidates_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", candidates_arraysize, k);
    this->candidates[k] = candidates;
}

void NotifyVote::setContextArgumentsArraySize(unsigned int size)
{
    double *contextArguments2 = (size==0) ? nullptr : new double[size];
    unsigned int sz = contextArguments_arraysize < size ? contextArguments_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        contextArguments2[i] = this->contextArguments[i];
    for (unsigned int i=sz; i<size; i++)
        contextArguments2[i] = 0;
    contextArguments_arraysize = size;
    delete [] this->contextArguments;
    this->contextArguments = contextArguments2;
}

unsigned int NotifyVote::getContextArgumentsArraySize() const
{
    return contextArguments_arraysize;
}

double NotifyVote::getContextArguments(unsigned int k) const
{
    if (k>=contextArguments_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", contextArguments_arraysize, k);
    return this->contextArguments[k];
}

void NotifyVote::setContextArguments(unsigned int k, double contextArguments)
{
    if (k>=contextArguments_arraysize) throw omnetpp::cRuntimeError("Array of size %d indexed by %d", contextArguments_arraysize, k);
    this->contextArguments[k] = contextArguments;
}

int NotifyVote::getContextId() const
{
    return this->contextId;
}

void NotifyVote::setContextId(int contextId)
{
    this->contextId = contextId;
}

class NotifyVoteDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    NotifyVoteDescriptor();
    virtual ~NotifyVoteDescriptor();

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

Register_ClassDescriptor(NotifyVoteDescriptor)

NotifyVoteDescriptor::NotifyVoteDescriptor() : omnetpp::cClassDescriptor("NotifyVote", "NegotiationMessage")
{
    propertynames = nullptr;
}

NotifyVoteDescriptor::~NotifyVoteDescriptor()
{
    delete[] propertynames;
}

bool NotifyVoteDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<NotifyVote *>(obj)!=nullptr;
}

const char **NotifyVoteDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *NotifyVoteDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int NotifyVoteDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount() : 3;
}

unsigned int NotifyVoteDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *NotifyVoteDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "candidates",
        "contextArguments",
        "contextId",
    };
    return (field>=0 && field<3) ? fieldNames[field] : nullptr;
}

int NotifyVoteDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "candidates")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "contextArguments")==0) return base+1;
    if (fieldName[0]=='c' && strcmp(fieldName, "contextId")==0) return base+2;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *NotifyVoteDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "double",
        "int",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : nullptr;
}

const char **NotifyVoteDescriptor::getFieldPropertyNames(int field) const
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

const char *NotifyVoteDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int NotifyVoteDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    NotifyVote *pp = (NotifyVote *)object; (void)pp;
    switch (field) {
        case 0: return pp->getCandidatesArraySize();
        case 1: return pp->getContextArgumentsArraySize();
        default: return 0;
    }
}

const char *NotifyVoteDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    NotifyVote *pp = (NotifyVote *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string NotifyVoteDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    NotifyVote *pp = (NotifyVote *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getCandidates(i));
        case 1: return double2string(pp->getContextArguments(i));
        case 2: return long2string(pp->getContextId());
        default: return "";
    }
}

bool NotifyVoteDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    NotifyVote *pp = (NotifyVote *)object; (void)pp;
    switch (field) {
        case 0: pp->setCandidates(i,string2long(value)); return true;
        case 1: pp->setContextArguments(i,string2double(value)); return true;
        case 2: pp->setContextId(string2long(value)); return true;
        default: return false;
    }
}

const char *NotifyVoteDescriptor::getFieldStructName(int field) const
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

void *NotifyVoteDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    NotifyVote *pp = (NotifyVote *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}


