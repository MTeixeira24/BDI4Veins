//
// Generated file, do not edit! Created by nedtool 5.4 from messages/market/AuctionStatusMessage.msg.
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
#include "AuctionStatusMessage_m.h"

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

Register_Class(AuctionStatusMessage)

AuctionStatusMessage::AuctionStatusMessage(const char *name, short kind) : ::MarketMessage(name,kind)
{
    this->auctionId = 0;
    this->context = 0;
    this->managerId = 0;
    this->winnerId = 0;
    this->auctionIteration = 0;
    this->payment = 0;
    this->wtpsum = 0;
    this->property = 0;
}

AuctionStatusMessage::AuctionStatusMessage(const AuctionStatusMessage& other) : ::MarketMessage(other)
{
    copy(other);
}

AuctionStatusMessage::~AuctionStatusMessage()
{
}

AuctionStatusMessage& AuctionStatusMessage::operator=(const AuctionStatusMessage& other)
{
    if (this==&other) return *this;
    ::MarketMessage::operator=(other);
    copy(other);
    return *this;
}

void AuctionStatusMessage::copy(const AuctionStatusMessage& other)
{
    this->auctionId = other.auctionId;
    this->context = other.context;
    this->managerId = other.managerId;
    this->winnerId = other.winnerId;
    this->auctionIteration = other.auctionIteration;
    this->payment = other.payment;
    this->wtpsum = other.wtpsum;
    this->property = other.property;
    this->propertyList = other.propertyList;
}

void AuctionStatusMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::MarketMessage::parsimPack(b);
    doParsimPacking(b,this->auctionId);
    doParsimPacking(b,this->context);
    doParsimPacking(b,this->managerId);
    doParsimPacking(b,this->winnerId);
    doParsimPacking(b,this->auctionIteration);
    doParsimPacking(b,this->payment);
    doParsimPacking(b,this->wtpsum);
    doParsimPacking(b,this->property);
    doParsimPacking(b,this->propertyList);
}

void AuctionStatusMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::MarketMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->auctionId);
    doParsimUnpacking(b,this->context);
    doParsimUnpacking(b,this->managerId);
    doParsimUnpacking(b,this->winnerId);
    doParsimUnpacking(b,this->auctionIteration);
    doParsimUnpacking(b,this->payment);
    doParsimUnpacking(b,this->wtpsum);
    doParsimUnpacking(b,this->property);
    doParsimUnpacking(b,this->propertyList);
}

int AuctionStatusMessage::getAuctionId() const
{
    return this->auctionId;
}

void AuctionStatusMessage::setAuctionId(int auctionId)
{
    this->auctionId = auctionId;
}

int AuctionStatusMessage::getContext() const
{
    return this->context;
}

void AuctionStatusMessage::setContext(int context)
{
    this->context = context;
}

int AuctionStatusMessage::getManagerId() const
{
    return this->managerId;
}

void AuctionStatusMessage::setManagerId(int managerId)
{
    this->managerId = managerId;
}

int AuctionStatusMessage::getWinnerId() const
{
    return this->winnerId;
}

void AuctionStatusMessage::setWinnerId(int winnerId)
{
    this->winnerId = winnerId;
}

int AuctionStatusMessage::getAuctionIteration() const
{
    return this->auctionIteration;
}

void AuctionStatusMessage::setAuctionIteration(int auctionIteration)
{
    this->auctionIteration = auctionIteration;
}

int AuctionStatusMessage::getPayment() const
{
    return this->payment;
}

void AuctionStatusMessage::setPayment(int payment)
{
    this->payment = payment;
}

int AuctionStatusMessage::getWtpsum() const
{
    return this->wtpsum;
}

void AuctionStatusMessage::setWtpsum(int wtpsum)
{
    this->wtpsum = wtpsum;
}

int AuctionStatusMessage::getProperty() const
{
    return this->property;
}

void AuctionStatusMessage::setProperty(int property)
{
    this->property = property;
}

IntList& AuctionStatusMessage::getPropertyList()
{
    return this->propertyList;
}

void AuctionStatusMessage::setPropertyList(const IntList& propertyList)
{
    this->propertyList = propertyList;
}

class AuctionStatusMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    AuctionStatusMessageDescriptor();
    virtual ~AuctionStatusMessageDescriptor();

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

Register_ClassDescriptor(AuctionStatusMessageDescriptor)

AuctionStatusMessageDescriptor::AuctionStatusMessageDescriptor() : omnetpp::cClassDescriptor("AuctionStatusMessage", "MarketMessage")
{
    propertynames = nullptr;
}

AuctionStatusMessageDescriptor::~AuctionStatusMessageDescriptor()
{
    delete[] propertynames;
}

bool AuctionStatusMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<AuctionStatusMessage *>(obj)!=nullptr;
}

const char **AuctionStatusMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *AuctionStatusMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int AuctionStatusMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount() : 9;
}

unsigned int AuctionStatusMessageDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *AuctionStatusMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "auctionId",
        "context",
        "managerId",
        "winnerId",
        "auctionIteration",
        "payment",
        "wtpsum",
        "property",
        "propertyList",
    };
    return (field>=0 && field<9) ? fieldNames[field] : nullptr;
}

int AuctionStatusMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='a' && strcmp(fieldName, "auctionId")==0) return base+0;
    if (fieldName[0]=='c' && strcmp(fieldName, "context")==0) return base+1;
    if (fieldName[0]=='m' && strcmp(fieldName, "managerId")==0) return base+2;
    if (fieldName[0]=='w' && strcmp(fieldName, "winnerId")==0) return base+3;
    if (fieldName[0]=='a' && strcmp(fieldName, "auctionIteration")==0) return base+4;
    if (fieldName[0]=='p' && strcmp(fieldName, "payment")==0) return base+5;
    if (fieldName[0]=='w' && strcmp(fieldName, "wtpsum")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "property")==0) return base+7;
    if (fieldName[0]=='p' && strcmp(fieldName, "propertyList")==0) return base+8;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *AuctionStatusMessageDescriptor::getFieldTypeString(int field) const
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
        "int",
        "int",
        "int",
        "int",
        "IntList",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : nullptr;
}

const char **AuctionStatusMessageDescriptor::getFieldPropertyNames(int field) const
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

const char *AuctionStatusMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int AuctionStatusMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    AuctionStatusMessage *pp = (AuctionStatusMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *AuctionStatusMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    AuctionStatusMessage *pp = (AuctionStatusMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string AuctionStatusMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    AuctionStatusMessage *pp = (AuctionStatusMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getAuctionId());
        case 1: return long2string(pp->getContext());
        case 2: return long2string(pp->getManagerId());
        case 3: return long2string(pp->getWinnerId());
        case 4: return long2string(pp->getAuctionIteration());
        case 5: return long2string(pp->getPayment());
        case 6: return long2string(pp->getWtpsum());
        case 7: return long2string(pp->getProperty());
        case 8: {std::stringstream out; out << pp->getPropertyList(); return out.str();}
        default: return "";
    }
}

bool AuctionStatusMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    AuctionStatusMessage *pp = (AuctionStatusMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setAuctionId(string2long(value)); return true;
        case 1: pp->setContext(string2long(value)); return true;
        case 2: pp->setManagerId(string2long(value)); return true;
        case 3: pp->setWinnerId(string2long(value)); return true;
        case 4: pp->setAuctionIteration(string2long(value)); return true;
        case 5: pp->setPayment(string2long(value)); return true;
        case 6: pp->setWtpsum(string2long(value)); return true;
        case 7: pp->setProperty(string2long(value)); return true;
        default: return false;
    }
}

const char *AuctionStatusMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 8: return omnetpp::opp_typename(typeid(IntList));
        default: return nullptr;
    };
}

void *AuctionStatusMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    AuctionStatusMessage *pp = (AuctionStatusMessage *)object; (void)pp;
    switch (field) {
        case 8: return (void *)(&pp->getPropertyList()); break;
        default: return nullptr;
    }
}


