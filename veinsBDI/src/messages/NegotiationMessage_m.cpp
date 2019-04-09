//
// Generated file, do not edit! Created by nedtool 5.4 from messages/NegotiationMessage.msg.
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
#include "NegotiationMessage_m.h"

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

Register_Class(NegotiationMessage)

NegotiationMessage::NegotiationMessage(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->vehicleId = 0;
    this->destinationId = 0;
    this->platoonId = 0;
    this->forWholePlatoon = false;
    this->messageId = 0;
    this->replyMessageId = 0;
    this->messageType = 0;
}

NegotiationMessage::NegotiationMessage(const NegotiationMessage& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

NegotiationMessage::~NegotiationMessage()
{
}

NegotiationMessage& NegotiationMessage::operator=(const NegotiationMessage& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void NegotiationMessage::copy(const NegotiationMessage& other)
{
    this->vehicleId = other.vehicleId;
    this->destinationId = other.destinationId;
    this->externalId = other.externalId;
    this->platoonId = other.platoonId;
    this->targets = other.targets;
    this->forWholePlatoon = other.forWholePlatoon;
    this->messageId = other.messageId;
    this->replyMessageId = other.replyMessageId;
    this->messageType = other.messageType;
}

void NegotiationMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->vehicleId);
    doParsimPacking(b,this->destinationId);
    doParsimPacking(b,this->externalId);
    doParsimPacking(b,this->platoonId);
    doParsimPacking(b,this->targets);
    doParsimPacking(b,this->forWholePlatoon);
    doParsimPacking(b,this->messageId);
    doParsimPacking(b,this->replyMessageId);
    doParsimPacking(b,this->messageType);
}

void NegotiationMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->vehicleId);
    doParsimUnpacking(b,this->destinationId);
    doParsimUnpacking(b,this->externalId);
    doParsimUnpacking(b,this->platoonId);
    doParsimUnpacking(b,this->targets);
    doParsimUnpacking(b,this->forWholePlatoon);
    doParsimUnpacking(b,this->messageId);
    doParsimUnpacking(b,this->replyMessageId);
    doParsimUnpacking(b,this->messageType);
}

int NegotiationMessage::getVehicleId() const
{
    return this->vehicleId;
}

void NegotiationMessage::setVehicleId(int vehicleId)
{
    this->vehicleId = vehicleId;
}

int NegotiationMessage::getDestinationId() const
{
    return this->destinationId;
}

void NegotiationMessage::setDestinationId(int destinationId)
{
    this->destinationId = destinationId;
}

const char * NegotiationMessage::getExternalId() const
{
    return this->externalId.c_str();
}

void NegotiationMessage::setExternalId(const char * externalId)
{
    this->externalId = externalId;
}

int NegotiationMessage::getPlatoonId() const
{
    return this->platoonId;
}

void NegotiationMessage::setPlatoonId(int platoonId)
{
    this->platoonId = platoonId;
}

IntSet& NegotiationMessage::getTargets()
{
    return this->targets;
}

void NegotiationMessage::setTargets(const IntSet& targets)
{
    this->targets = targets;
}

bool NegotiationMessage::getForWholePlatoon() const
{
    return this->forWholePlatoon;
}

void NegotiationMessage::setForWholePlatoon(bool forWholePlatoon)
{
    this->forWholePlatoon = forWholePlatoon;
}

int NegotiationMessage::getMessageId() const
{
    return this->messageId;
}

void NegotiationMessage::setMessageId(int messageId)
{
    this->messageId = messageId;
}

int NegotiationMessage::getReplyMessageId() const
{
    return this->replyMessageId;
}

void NegotiationMessage::setReplyMessageId(int replyMessageId)
{
    this->replyMessageId = replyMessageId;
}

int NegotiationMessage::getMessageType() const
{
    return this->messageType;
}

void NegotiationMessage::setMessageType(int messageType)
{
    this->messageType = messageType;
}

class NegotiationMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    NegotiationMessageDescriptor();
    virtual ~NegotiationMessageDescriptor();

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

Register_ClassDescriptor(NegotiationMessageDescriptor)

NegotiationMessageDescriptor::NegotiationMessageDescriptor() : omnetpp::cClassDescriptor("NegotiationMessage", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

NegotiationMessageDescriptor::~NegotiationMessageDescriptor()
{
    delete[] propertynames;
}

bool NegotiationMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<NegotiationMessage *>(obj)!=nullptr;
}

const char **NegotiationMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *NegotiationMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int NegotiationMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount() : 9;
}

unsigned int NegotiationMessageDescriptor::getFieldTypeFlags(int field) const
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
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *NegotiationMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "vehicleId",
        "destinationId",
        "externalId",
        "platoonId",
        "targets",
        "forWholePlatoon",
        "messageId",
        "replyMessageId",
        "messageType",
    };
    return (field>=0 && field<9) ? fieldNames[field] : nullptr;
}

int NegotiationMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleId")==0) return base+0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destinationId")==0) return base+1;
    if (fieldName[0]=='e' && strcmp(fieldName, "externalId")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "platoonId")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "targets")==0) return base+4;
    if (fieldName[0]=='f' && strcmp(fieldName, "forWholePlatoon")==0) return base+5;
    if (fieldName[0]=='m' && strcmp(fieldName, "messageId")==0) return base+6;
    if (fieldName[0]=='r' && strcmp(fieldName, "replyMessageId")==0) return base+7;
    if (fieldName[0]=='m' && strcmp(fieldName, "messageType")==0) return base+8;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *NegotiationMessageDescriptor::getFieldTypeString(int field) const
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
        "string",
        "int",
        "IntSet",
        "bool",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : nullptr;
}

const char **NegotiationMessageDescriptor::getFieldPropertyNames(int field) const
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

const char *NegotiationMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
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

int NegotiationMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    NegotiationMessage *pp = (NegotiationMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *NegotiationMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    NegotiationMessage *pp = (NegotiationMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string NegotiationMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    NegotiationMessage *pp = (NegotiationMessage *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->getVehicleId());
        case 1: return long2string(pp->getDestinationId());
        case 2: return oppstring2string(pp->getExternalId());
        case 3: return long2string(pp->getPlatoonId());
        case 4: {std::stringstream out; out << pp->getTargets(); return out.str();}
        case 5: return bool2string(pp->getForWholePlatoon());
        case 6: return long2string(pp->getMessageId());
        case 7: return long2string(pp->getReplyMessageId());
        case 8: return long2string(pp->getMessageType());
        default: return "";
    }
}

bool NegotiationMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    NegotiationMessage *pp = (NegotiationMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setVehicleId(string2long(value)); return true;
        case 1: pp->setDestinationId(string2long(value)); return true;
        case 2: pp->setExternalId((value)); return true;
        case 3: pp->setPlatoonId(string2long(value)); return true;
        case 5: pp->setForWholePlatoon(string2bool(value)); return true;
        case 6: pp->setMessageId(string2long(value)); return true;
        case 7: pp->setReplyMessageId(string2long(value)); return true;
        case 8: pp->setMessageType(string2long(value)); return true;
        default: return false;
    }
}

const char *NegotiationMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 4: return omnetpp::opp_typename(typeid(IntSet));
        default: return nullptr;
    };
}

void *NegotiationMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    NegotiationMessage *pp = (NegotiationMessage *)object; (void)pp;
    switch (field) {
        case 4: return (void *)(&pp->getTargets()); break;
        default: return nullptr;
    }
}


