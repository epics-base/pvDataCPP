/*
 * Copyright information and license terms for this software can be
 * found in the file LICENSE that is included with the distribution
 */

#include <string>
#include <sstream>

#include <epicsMutex.h>

#define epicsExportSharedSymbols

#include <pv/pvData.h>
#include <pv/lock.h>
#include <pv/createRequest.h>

using namespace epics::pvData;
using std::ostringstream;
using std::tr1::static_pointer_cast;
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace epics { namespace pvData {

static PVDataCreatePtr pvDataCreate = getPVDataCreate();
static FieldCreatePtr fieldCreate = getFieldCreate();

class CreateRequestImpl : public CreateRequest {
private:

    struct Node 
    {
        string name;
        vector<Node> nodes;
        Node(string name)
        : name(name)
        {}
    };

    struct OptionPair
    {
         string name;
         string value;
         OptionPair(string name,string value)
         : name(name),
           value(value)
         {}
    };


    vector<OptionPair> optionList;
    string fullFieldName;


public:
    CreateRequestImpl()
    {
        fullFieldName = "";
    }
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
             message = request + " mismatched {}";
             throw std::logic_error("message");
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
                     message = request + " mismatched []";
                     throw std::logic_error("message");
                }
                return i;
            }
        }
        message = request + " missing ]";
        throw std::logic_error("message");
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
             if(request[ind]=='.') {
                 ++ind;
                 continue;
             }
             if(ind>=maxind) break;
             ++ind;
        }
        return request.size();
    }

    vector<string> split(string const & commaSeparatedList) {
        string::size_type numValues = 1;
        string::size_type index=0;
        while(true) {
            string::size_type pos = commaSeparatedList.find(',',index);
            if(pos==string::npos) break;
            numValues++;
            index = pos +1;
	}
        vector<string> valueList(numValues,"");
        index=0;
        for(size_t i=0; i<numValues; i++) {
            size_t pos = commaSeparatedList.find(',',index);
            string value = commaSeparatedList.substr(index,pos-index);
            valueList[i] = value;
            index = pos +1;
        }
        return valueList;
    }

    Node createRequestOptions(
        string const & request)
    {
        if(request.length()<=1) {
            throw std::logic_error("logic error empty options");
        }
        vector<Node> top;
        vector<string> items = split(request);

        size_t nitems = items.size();
        for(size_t j=0; j<nitems; j++) {
            string item = items[j];
            size_t equals = item.find('=');
            if(equals==string::npos || equals==0) {
                message = item + " illegal option " + request;
                throw std::logic_error("message");
            }
            top.push_back(Node(item.substr(0,equals)));
            string name = fullFieldName + "._options." + item.substr(0,equals);
            string value = item.substr(equals+1);
            optionList.push_back(OptionPair(name,value));
        }
        Node node("_options");
        node.nodes = top;
        return node;
    }


    void createSubNode(Node &node,string const & crequest)
    {
        string request = crequest;
        size_t end = 0;
        for(size_t i=0; i<request.size(); ++i) {
            if(request[i]=='[') { end = i; break;}
            if(request[i]=='.') { end = i; break;}
            if(request[i]=='{') { end = i; break;}
            if(request[i]==',') { end = i; break;}
        }
        char chr = request[end];
        Node optionNode("");
        if(chr=='[') {
            string saveFullName = fullFieldName;
            fullFieldName += "." + request.substr(0,end);
            size_t endBracket = findMatchingBracket(request,end);
            string options = request.substr(end+1,endBracket -end -1);
            optionNode = createRequestOptions(options);
            fullFieldName = saveFullName;
            size_t next = endBracket+1;
            if(next<request.size()) {
                request = request.substr(0, end) + request.substr(endBracket+1);
            } else {
                request = request.substr(0, end);
            }
            end = 0;
            for(size_t i=0; i<request.size(); ++i) {
                if(request[i]=='.') { end = i; break;}
                if(request[i]=='{') { end = i; break;}
                if(request[i]==',') { end = i; break;}
            }
            chr = request[end];
        }
        if(end==0) end = request.size();
        string name = request.substr(0,end);
        if(name.size()<1) {
            throw std::logic_error("null field name " + request);
        }
        string saveFullName = fullFieldName;
        fullFieldName += "." + name;
        if(end==request.size()) {
            Node subNode(name);
            if(optionNode.name.size()>0) subNode.nodes.push_back(optionNode);
            node.nodes.push_back(subNode);
            fullFieldName = saveFullName;
            return;
        }
        if(chr==',') {
            Node subNode(name);
            if(optionNode.name.size()>0) subNode.nodes.push_back(optionNode);
            node.nodes.push_back(subNode);
            string rest = request.substr(end+1);
            fullFieldName = saveFullName;
            createSubNode(node,rest);
            return;
        }
        if(chr=='.') {
            request = request.substr(end+1);
            if(request.size()==string::npos || request.size()<1) {
                throw std::logic_error("null field name " + request);
            }
            Node subNode(name);
            if(optionNode.name.size()>0) subNode.nodes.push_back(optionNode);
            size_t endField = findEndField(request);
            string subRequest = request.substr(0, endField);
            createSubNode(subNode,subRequest);
            node.nodes.push_back(subNode);
            size_t next = endField+1;
            if(next>=request.size()) {
                fullFieldName = saveFullName;
                return;
            }
            request = request.substr(next);
            fullFieldName = saveFullName;
            createSubNode(node,request);
            return;
        }
        if(chr=='{') {
            size_t endBrace = findEndField(request);
            if((end+1)>=(endBrace-1)) {
                throw std::logic_error("illegal syntax " + request);
            }
            string subRequest = request.substr(end+1,endBrace-1 -end -1);
            if(subRequest.size()<1) {
                throw std::logic_error("empty {} " + request);
            }
            Node subNode(name);
            if(optionNode.name.size()>0) subNode.nodes.push_back(optionNode);
            createSubNode(subNode,subRequest);
            node.nodes.push_back(subNode);
            size_t next = endBrace + 1;
            if(next>=request.size()) {
                fullFieldName = saveFullName;
                return;
            }
            request = request.substr(next);
            fullFieldName = saveFullName;
            createSubNode(node,request);
            return;
        }
        throw std::logic_error("logic error");
    }

    FieldConstPtr createSubStructure(vector<Node> & nodes)
    {
        size_t num = nodes.size();
        StringArray names(num);
        FieldConstPtrArray fields(num);
        for(size_t i=0; i<num; ++i) {
            Node node = nodes[i];
            names[i] = node.name;
            if(node.name.compare("_options")==0) {
                fields[i] = createOptions(node.nodes);
            } else {
                vector<Node> subNode = node.nodes;
                if(subNode.empty()) {
                    fields[i] = fieldCreate->createStructure();
                } else {
                    fields[i] = createSubStructure(subNode);
                }
            }
        }
        StructureConstPtr structure = fieldCreate->createStructure(
            names, fields);
        return structure;
    }

    StructureConstPtr createOptions(vector<Node> &nodes)
    {
        size_t num = nodes.size();
        StringArray names(num);
        FieldConstPtrArray fields(num);
        for(size_t i=0; i<num; ++i) {
            Node node = nodes[i];
            names[i] = node.name;
            fields[i] = fieldCreate->createScalar(pvString);
        }
        StructureConstPtr structure = fieldCreate->createStructure(names, fields);
        return structure;
    }

public:

    virtual PVStructurePtr createRequest(
        string const & crequest)
    {
        try {
            string request = crequest;
            if (!request.empty()) removeBlanks(request);
            if (request.empty())
            {
                return pvDataCreate->createPVStructure(fieldCreate->createStructure());
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
                 request = "field(" + request + ")";
                 offsetField = request.find("field(");
            }
            int numParan = 0;
            int numBrace = 0;
            int numBracket = 0;
            for(size_t i=0; i< request.length() ; ++i) {
                char chr = request[i];
                if(chr=='(') numParan++;
                if(chr==')') numParan--;
                if(chr=='{') numBrace++;
                if(chr=='}') numBrace--;
                if(chr=='[') numBracket++;
                if(chr==']') numBracket--;
            }
            if(numParan!=0) {
                ostringstream oss;
                oss << "mismatched () " << numParan;
                message = oss.str();
                return PVStructurePtr();
            }
            if(numBrace!=0) {
                ostringstream oss;
                oss << "mismatched {} " << numBrace;
                message = oss.str();
                return PVStructurePtr();
            }
            if(numBracket!=0) {
                ostringstream oss;
                oss << "mismatched [] " << numBracket;
                message = oss.str();
                return PVStructurePtr();
            }
            vector<Node> top;
            try {
                if(offsetRecord!=string::npos) {
                    fullFieldName = "record";
                    size_t openBracket = request.find('[', offsetRecord);
                    size_t closeBracket = request.find(']', openBracket);
                    if(closeBracket==string::npos) {
                        message = request.substr(offsetRecord) +
                            "record[ does not have matching ]";
                        return PVStructurePtr();
                    }
                    if(closeBracket-openBracket > 3) {
                        Node node("record");
                        Node optNode = createRequestOptions(
                                request.substr(openBracket+1,closeBracket-openBracket-1));
                        node.nodes.push_back(optNode);
                        top.push_back(node);
                    }
                }
                if(offsetField!=string::npos) {
                    fullFieldName = "field";
                    Node node("field");
                    size_t openParan = request.find('(', offsetField);
                    size_t closeParan = request.find(')', openParan);
                    if(closeParan==string::npos) {
                        message = request.substr(offsetField)
                                + " field( does not have matching )";
                        return PVStructurePtr();
                    }
                    if(closeParan>openParan+1) {
                        createSubNode(node,request.substr(openParan+1,closeParan-openParan-1));
                    }
                    top.push_back(node);
                }
                if(offsetGetField!=string::npos) {
                    fullFieldName = "getField";
                    Node node("getField");
                    size_t openParan = request.find('(', offsetGetField);
                    size_t closeParan = request.find(')', openParan);
                    if(closeParan==string::npos) {
                        message = request.substr(offsetField)
                                + " getField( does not have matching )";
                        return PVStructurePtr();
                    }
                    if(closeParan>openParan+1) {
                        createSubNode(node,request.substr(openParan+1,closeParan-openParan-1));
                    }
                    top.push_back(node);
                }
                if(offsetPutField!=string::npos) {
                    fullFieldName = "putField";
                    Node node("putField");
                    size_t openParan = request.find('(', offsetPutField);
                    size_t closeParan = request.find(')', openParan);
                    if(closeParan==string::npos) {
                        message = request.substr(offsetField)
                                + " putField( does not have matching )";
                        return PVStructurePtr();
                    }
                    if(closeParan>openParan+1) {
                        createSubNode(node,request.substr(openParan+1,closeParan-openParan-1));
                    }
                    top.push_back(node);
                }
            } catch (std::exception &e) {
                string xxx = e.what();
                message = "while creating Structure exception " + xxx;
                return PVStructurePtr();
            }
            size_t num = top.size();
            StringArray names(num);
            FieldConstPtrArray fields(num);
            for(size_t i=0; i<num; ++i) {
                Node node = top[i];
                names[i] = node.name;
                vector<Node> subNode = node.nodes;
                if(subNode.empty()) {
                    fields[i] = fieldCreate->createStructure();
                } else {
                    fields[i] = createSubStructure(subNode);
                }
            }
            StructureConstPtr structure = fieldCreate->createStructure(names, fields);
            if(!structure) throw std::invalid_argument("bad request " + crequest);
            PVStructurePtr pvStructure = pvDataCreate->createPVStructure(structure);
            for(size_t i=0; i<optionList.size(); ++i) {
                OptionPair pair = optionList[i];
                string name = pair.name;
                string value = pair.value;
                PVStringPtr pvField = pvStructure->getSubField<PVString>(name);
                if(!pvField) throw std::invalid_argument("bad request " + crequest);
                pvField->put(value);
            }
            optionList.clear();
            return pvStructure;
        } catch (std::exception &e) {
             message = e.what();
             return PVStructurePtr();
        }
    }


};

CreateRequest::shared_pointer CreateRequest::create()
{
    CreateRequest::shared_pointer createRequest(new CreateRequestImpl());
    return createRequest;
}

}}

