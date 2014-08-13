/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * pvAccessCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */

#include <string>
#include <sstream>

#define epicsExportSharedSymbols

#include <pv/pvData.h>
#include <pv/lock.h>
#include <pv/createRequest.h>

using namespace epics::pvData;
using std::tr1::static_pointer_cast;
using std::cout;
using std::endl;
using std::string;

namespace epics { namespace pvData {

static PVDataCreatePtr pvDataCreate = getPVDataCreate();
static FieldCreatePtr fieldCreate = getFieldCreate();

class CreateRequestImpl : public CreateRequest {
private:

    void removeBlanks(string& str)
    {
        while(true) {
            string::size_type pos = str.find_first_of(' ');
            if(pos==string::npos) return;
            str.erase(pos,1);
        }
    }

    size_t findMatchingBrace(string& request, size_t index, int numOpen) {
        size_t openBrace = request.find('{', index+1);
        size_t closeBrace = request.find('}', index+1);
        if(openBrace == string::npos && closeBrace == string::npos){
             message = request + " missing }";
             return string::npos;
        }
        if (openBrace != string::npos && openBrace!=0) {
            if(openBrace<closeBrace) return findMatchingBrace(request,openBrace,numOpen+1);
            if(numOpen==1) return closeBrace;
            return findMatchingBrace(request,closeBrace,numOpen-1);
        }
        if(numOpen==1) return closeBrace;
        return findMatchingBrace(request,closeBrace,numOpen-1);
    }

    size_t findMatchingBracket(string& request, size_t index) {
        for(size_t i=index+1; i< request.size(); ++i) {
            if(request[i] == ']') {
                if(i==index+1) {
                     message = request + " illegal []";
                     return string::npos;
                }
                return i;
            }
        }
         message = request + " missing ]";
         return string::npos;
    }

    size_t findEndField(string& request) {
        size_t ind = 0;
        size_t maxind = request.size() -1;
        while(true) {
             if(request[ind]==',') return ind;
             if(request[ind]=='[') {
                 size_t closeBracket = findMatchingBracket(request,ind);
                 if(closeBracket==string::npos) return closeBracket;
                 ind = closeBracket;
                 continue;
             }
             if(request[ind]=='{') {
                 size_t closeBrace = findMatchingBrace(request,ind,1);
                 if(closeBrace==string::npos) return closeBrace;
                 if(ind>=request.size()) return request.size();
                 ind = closeBrace;
                 continue;
             }
             if(ind>=maxind) break;
             ++ind;
        }
        return request.size();
    }

    std::vector<string> split(string const & commaSeparatedList) {
        string::size_type numValues = 1;
        string::size_type index=0;
        while(true) {
            string::size_type pos = commaSeparatedList.find(',',index);
            if(pos==string::npos) break;
            numValues++;
            index = pos +1;
	}
        std::vector<string> valueList(numValues,"");
        index=0;
        for(size_t i=0; i<numValues; i++) {
            size_t pos = commaSeparatedList.find(',',index);
            string value = commaSeparatedList.substr(index,pos-index);
            valueList[i] = value;
            index = pos +1;
        }
        return valueList;
    }

    StructureConstPtr createRequestOptions(
        string request)
    {
        if(request.length()<=1) return StructureConstPtr();
        std::vector<string> items = split(request);
        size_t nitems = items.size();
        StringArray fieldNames(nitems);
        FieldConstPtrArray fields(nitems);
        for(size_t j=0; j<nitems; j++) {
            string item = items[j];
            size_t equals = item.find('=');
            if(equals==string::npos || equals==0) {
                message = item + " illegal option";
                StructureConstPtr xxx;
                return xxx;
            }
            fieldNames[j] = item.substr(0,equals);
            fields[j] = fieldCreate->createScalar(pvString);
        }
        StringArray names(1);
        FieldConstPtrArray field(1);
        names[0] = "_options";
        field[0] = fieldCreate->createStructure(fieldNames,fields);
        return fieldCreate->createStructure(names,field);
    }

    void initRequestOptions(
        PVStructurePtr const & pvParent,
        string request)
    {
        if(request.length()<=1) return;
        std::vector<string> items = split(request);
        size_t nitems = items.size();
        for(size_t j=0; j<nitems; j++) {
            string item = items[j];
            size_t equals = item.find('=');
            string name = "_options." + item.substr(0,equals);
            string value = item.substr(equals+1);
            PVStringPtr pvValue = pvParent->getSubField<PVString>(name);
            pvValue->put(value);
        }
    }

    StructureConstPtr createSubFieldRequest(
        StructureConstPtr parent,
        string request)
    {
        if(request.length()<=0) return parent;
        size_t period = request.find('.');
        size_t openBracket = request.find('[');
        size_t openBrace = request.find('{');

        // name only
        if(period==string::npos
        && openBracket==string::npos
        && openBrace==string::npos)
        {
            StructureConstPtr subField = fieldCreate->createStructure();
            parent = fieldCreate->appendField(parent,request,subField);
            return parent;
        }

        // period is first
        if(period!=string::npos
        && (openBracket==string::npos || period<openBracket)
        && (openBrace==string::npos || period<openBrace) )
        {
            string fieldName = request.substr(0,period);
            StructureConstPtr subField = fieldCreate->createStructure();
            subField = createSubFieldRequest(subField,request.substr(period+1));
            if(!subField) return subField;
            parent = fieldCreate->appendField(parent,fieldName,subField);
            return parent;
        }

        // brace before [ or .
        if(openBrace!=string::npos
        && (openBracket==string::npos || openBrace<openBracket) )
        {
            string fieldName = request.substr(0,openBrace);
            size_t closeBrace = findMatchingBrace(request,openBrace,1);
            if(closeBrace==string::npos) return StructureConstPtr();
            size_t nextChar = closeBrace+1;
            if(nextChar>= request.size()) nextChar = string::npos;
            if(nextChar!=string::npos) {
                message = request + " syntax error " + request[nextChar] + " after } illegal";
                return StructureConstPtr();
            }
            StructureConstPtr subField = fieldCreate->createStructure();
            string subRequest = request.substr(openBrace+1,closeBrace-openBrace-1);
            subField = createFieldRequest(subField,subRequest);
            if(!subField) return subField;
            parent = fieldCreate->appendField(parent,fieldName,subField);
            return parent;
        }

        // [ is before brace or .
        if(openBracket!=string::npos
        && (openBrace==string::npos || openBracket<openBrace) )
        {
            string fieldName = request.substr(0,openBracket);
            size_t closeBracket = findMatchingBracket(request,openBracket);
            if(closeBracket==string::npos) return StructureConstPtr();
            size_t nextChar = closeBracket+1;
            if(nextChar>= request.size()) nextChar = string::npos;
            if(nextChar==string::npos) {
                StringArray fieldNames(1);
                FieldConstPtrArray fields(1);
                fieldNames[0] = "_options";
                fields[0] = createRequestOptions(
                    request.substr(openBracket+1,closeBracket-openBracket-1));
                StructureConstPtr subField = fieldCreate->createStructure(fieldNames,fields);
                parent = fieldCreate->appendField(parent,fieldName,subField);
                return parent;
            }
            if(request[nextChar]=='.') {
                StructureConstPtr subField = fieldCreate->createStructure();
                subField = createSubFieldRequest(subField,request.substr(nextChar+1));
                if(!subField) return StructureConstPtr();
                if(subField->getNumberFields()!=1) {
                    message = request + " logic error createSubFieldRequest openBracket subField";
                    return StructureConstPtr();
                }
                StringArray fieldNames(2);
                FieldConstPtrArray fields(2);
                fieldNames[0] = "_options";
                fields[0] = createRequestOptions(
                    request.substr(openBracket+1,closeBracket-openBracket-1));
                fieldNames[1] = subField->getFieldNames()[0];
                fields[1] = subField;
                subField = fieldCreate->createStructure(fieldNames,fields);
                parent = fieldCreate->appendField(parent,fieldName,subField);
                return parent;
            }
            if(request[nextChar]=='{') {
                size_t closeBrace = findMatchingBrace(request,openBrace,1);
                if(closeBrace==string::npos) return StructureConstPtr();
                StructureConstPtr subField = fieldCreate->createStructure();
                string subRequest = request.substr(openBrace+1,closeBrace-openBrace-1);
                subField = createFieldRequest(subField,subRequest);
                if(!subField) return subField;
                size_t numSub = subField->getNumberFields();
                StringArray fieldNames(numSub + 1);
                FieldConstPtrArray fields(numSub+1);
                fieldNames[0] = "_options";
                fields[0] = createRequestOptions(
                    request.substr(openBracket+1,closeBracket-openBracket-1));
                StringArray subNames = subField->getFieldNames();
                FieldConstPtrArray subFields = subField->getFields();
                for(size_t i=0; i<numSub; ++i) {
                    fieldNames[i+1] = subNames[i];
                    fields[i+1] = subFields[i];
                }
                subField = fieldCreate->appendFields(parent,fieldNames,fields);
                parent = fieldCreate->appendField(parent,fieldName,subField);
                return parent;
            }
        }
        message = request + " logic error createSubFieldRequest";
        return StructureConstPtr();
    }


    StructureConstPtr createFieldRequest(
        StructureConstPtr parent,
        string request)
    {
        size_t length = request.length();
        if(length<=0) return parent;
        size_t end = findEndField(request);
        if(end==string::npos) return StructureConstPtr();
        StringArray fieldNames;
        FieldConstPtrArray fields;
        StructureConstPtr subField = fieldCreate->createStructure();
        subField = createSubFieldRequest(subField,request.substr(0,end));
        if(!subField) return subField;
        fieldNames.push_back(subField->getFieldNames()[0]);
        fields.push_back(subField->getFields()[0]);
        if(end!=length) {
            if(request[end]!=',') {
                 message = request;
                 message += " expected char ";
                 message += length;
                 message += " to be ,";
                 return StructureConstPtr();
            }
            StructureConstPtr nextSubField = fieldCreate->createStructure();
            nextSubField = createFieldRequest(nextSubField,request.substr(end+1));
            if(!nextSubField) return nextSubField;
            size_t numFields = nextSubField->getNumberFields();
            StringArray subNames = nextSubField->getFieldNames();
            FieldConstPtrArray subFields = nextSubField->getFields();
            for(size_t i=0; i<numFields; ++i) {
                fieldNames.push_back(subNames[i]);
                fields.push_back(subFields[i]);
            }
        }
        parent = fieldCreate->appendFields(parent,fieldNames,fields);
        return parent;
    }

    void initSubFieldOptions(
        PVStructurePtr const & pvParent,
        string request)
    {
        if(request.length()<=0) return;
        size_t period = request.find('.');
        size_t openBracket = request.find('[');
        size_t openBrace = request.find('{');
        // name only
        if(period==string::npos
        && openBracket==string::npos
        && openBrace==string::npos)
        {
             return;
        }

        // period is first
        if(period!=string::npos
        && (openBracket==string::npos || period<openBracket)
        && (openBrace==string::npos || period<openBrace) )
        {
            PVStructurePtr pvSubField = static_pointer_cast<PVStructure>(pvParent->getPVFields()[0]);
            initSubFieldOptions(pvSubField,request.substr(period+1));
            return;
        }

        // brace before [ or .
        if(openBrace!=string::npos
        && (openBracket==string::npos || openBrace<openBracket) )
        {
            PVStructurePtr pvSubField = static_pointer_cast<PVStructure>(pvParent->getPVFields()[0]);
            size_t closeBrace = findMatchingBrace(request,openBrace,1);
            string subRequest = request.substr(openBrace+1,closeBrace-openBrace-1);
            initFieldOptions(pvSubField,subRequest);
            return;
        }
        PVStructurePtr pvOptions = pvParent->getSubField<PVStructure>("_options");
        if(!pvOptions) throw std::logic_error("initSubFieldOptions pvOptions NULL");
        size_t closeBracket = findMatchingBracket(request,openBracket);
        initRequestOptions(pvOptions,request.substr(openBracket+1,closeBracket-openBracket-1));
        size_t nextChar = closeBracket+1;
        if(nextChar>= request.size()) nextChar = string::npos;
        if(nextChar==string::npos) return;
        if(request[nextChar]=='.') {
            PVStructurePtr pvSubField = static_pointer_cast<PVStructure>(pvParent->getPVFields()[1]);
            initSubFieldOptions(pvSubField,request.substr(nextChar+1));
            return;
        }
        if(request[nextChar]!='{') throw std::logic_error("initSubFieldOptions request[nextChar]!='{'");
        size_t closeBrace = findMatchingBrace(request,openBrace,1);
        const PVFieldPtrArray &pvFields = pvParent->getPVFields();
        string subRequest = request.substr(openBrace+1,closeBrace-openBrace-1);
        for(size_t i=1; i<pvFields.size(); ++i) {
             PVStructurePtr pvSubField = static_pointer_cast<PVStructure>(pvFields[i]);
             size_t comma = subRequest.find(',');
             initSubFieldOptions(pvSubField, subRequest.substr(0,comma-1));
             subRequest = subRequest.substr(comma+1);
        }
    }

    void initFieldOptions(
        PVStructurePtr const & pvParent,
        string request)
    {
        if(request.find('[')==string::npos) return;
        size_t num = pvParent->getStructure()->getNumberFields();
        if(num==0) return;
        if(num==1) {
             initSubFieldOptions(pvParent,request);
             return;
        }
        size_t end = findEndField(request);
        size_t start = 0;
        for(size_t i=0; i<num; ++i) {
            PVStructurePtr pvSub = static_pointer_cast<PVStructure>(pvParent->getPVFields()[i]);
            string subRequest = request.substr(start, end - start);
            initSubFieldOptions(pvSub,subRequest);
            if(i==num-1) break;
            start = end +1;
            string xxx = request.substr(start);
            end += findEndField(xxx) + 1;
        }
    }


public:

    virtual PVStructure::shared_pointer createRequest(
        string const & crequest)
    {
    	string request = crequest;
        StructureConstPtr topStructure = fieldCreate->createStructure();

        if (!request.empty()) removeBlanks(request);
        if (request.empty())
        {
            PVFieldPtrArray pvFields;
            StringArray fieldNames;
            return pvDataCreate->createPVStructure(fieldNames,pvFields);
        }
        size_t offsetRecord = request.find("record[");
        size_t offsetField = request.find("field(");
        size_t offsetPutField = request.find("putField(");
        size_t offsetGetField = request.find("getField(");
        if(offsetRecord==string::npos
        && offsetField==string::npos
        && offsetPutField==string::npos
        && offsetGetField==string::npos)
        {
             request = "field(" + crequest + ")";
             offsetField = request.find("field(");
        }
        if (offsetRecord != string::npos) {
            size_t openBracket = request.find('[', offsetRecord);
            size_t closeBracket = request.find(']', openBracket);
            if(closeBracket == string::npos) {
                message = request.substr(offsetRecord)
                    + " record[ does not have matching ]";
                return PVStructurePtr();
            }
            StructureConstPtr structure = createRequestOptions(
                request.substr(openBracket+1,closeBracket-openBracket-1));
            if(!structure)
            {
                 return PVStructurePtr();
            }
            topStructure = fieldCreate->appendField(topStructure,"record",structure);
        }
        if (offsetField != string::npos) {
            size_t openBrace = request.find('(', offsetField);
            size_t closeBrace = request.find(')', openBrace);
            if(closeBrace == string::npos) {
                message = request.substr(offsetField)
                    + " field( does not have matching )";
                return PVStructurePtr();
            }
            StructureConstPtr structure = fieldCreate->createStructure();
            structure = createFieldRequest(structure,request.substr(openBrace+1,closeBrace-openBrace-1));
            if(!structure)
            {
                return PVStructurePtr();
            }
            topStructure = fieldCreate->appendField(topStructure,"field",structure);
        }
        if (offsetPutField != string::npos) {
            size_t openBrace = request.find('(', offsetPutField);
            size_t closeBrace = request.find(')', openBrace);
            if(closeBrace == string::npos) {
                message =  request.substr(offsetField)
                    + " putField( does not have matching )";
                return PVStructurePtr();
            }
            StructureConstPtr structure = fieldCreate->createStructure();
            structure = createFieldRequest(structure,request.substr(openBrace+1,closeBrace-openBrace-1));
            if(!structure)
            {
                return PVStructurePtr();
            }
            topStructure = fieldCreate->appendField(topStructure,"putField",structure);
        }
        if (offsetGetField != string::npos) {
            size_t openBrace = request.find('(', offsetGetField);
            size_t closeBrace = request.find(')', openBrace);
            if(closeBrace == string::npos) {
                message = request.substr(offsetField)
                    + " getField( does not have matching )";
                return PVStructurePtr();
            }
            StructureConstPtr structure = fieldCreate->createStructure();
            structure = createFieldRequest(structure,request.substr(openBrace+1,closeBrace-openBrace-1));
            if(!structure)
            {
                return PVStructurePtr();
            }
            topStructure = fieldCreate->appendField(topStructure,"getField",structure);
        }
        PVStructurePtr pvStructure = pvDataCreate->createPVStructure(topStructure);
        if (offsetRecord != string::npos) {
            size_t openBracket = request.find('[', offsetRecord);
            size_t closeBracket = request.find(']', openBracket);
            initRequestOptions(
                pvStructure->getSubField<PVStructure>("record"),
                request.substr(openBracket+1,closeBracket-openBracket-1));
        }
        if (offsetField != string::npos) {
            size_t openParam = request.find('(', offsetField);
            size_t closeParam = request.find(')', openParam);
            PVStructurePtr pvSub = pvStructure->getSubField<PVStructure>("field");
            if(pvSub->getStructure()->getNumberFields()==1) {
                pvSub = static_pointer_cast<PVStructure>(pvSub->getPVFields()[0]);
            }
            if(pvSub) initFieldOptions(pvSub,request.substr(openParam+1,closeParam-openParam-1));
        }
        if (offsetPutField != string::npos) {
            size_t openParam = request.find('(', offsetPutField);
            size_t closeParam = request.find(')', openParam);
            PVStructurePtr pvSub = pvStructure->getSubField<PVStructure>("putField");
            if(pvSub->getStructure()->getNumberFields()==1) {
                pvSub = static_pointer_cast<PVStructure>(pvSub->getPVFields()[0]);
            }
            if(pvSub) initFieldOptions(pvSub,request.substr(openParam+1,closeParam-openParam-1));
        }
        if (offsetGetField != string::npos) {
            size_t openParam = request.find('(', offsetGetField);
            size_t closeParam = request.find(')', openParam);
            PVStructurePtr pvSub = pvStructure->getSubField<PVStructure>("getField");
            if(pvSub->getStructure()->getNumberFields()==1) {
                pvSub = static_pointer_cast<PVStructure>(pvSub->getPVFields()[0]);
            }
            if(pvSub) initFieldOptions(pvSub,request.substr(openParam+1,closeParam-openParam-1));
        }
        return pvStructure;
    }

};

CreateRequest::shared_pointer CreateRequest::create()
{
    CreateRequest::shared_pointer createRequest(new CreateRequestImpl());
    return createRequest;
}

}}

