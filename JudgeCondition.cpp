#include "JudgeCondition.h"
bool Condition::TestRight(int content)
{
    stringstream tempcontent;
    tempcontent << value;
    int myContent;
    tempcontent >> myContent;
    
    switch (operate)
    {
        case Condition::OPERATOR_EQUAL:
            return content == myContent;
            break;
        case Condition::OPERATOR_NOT_EQUAL:
            return content != myContent;
            break;
        case Condition::OPERATOR_LESS:
            return content < myContent;
            break;
        case Condition::OPERATOR_MORE:
            return content > myContent;
            break;
        case Condition::OPERATOR_LESS_EQUAL:
            return content <= myContent;
            break;
        case Condition::OPERATOR_MORE_EQUAL:
            return content >= myContent;
            break;
        default:
            return true;
            break;
    }
}

bool Condition::TestRight(float content)
{
    stringstream temp_stream;
    temp_stream << value;
    float myContent;
    temp_stream >> myContent;
    
    switch (operate)
    {
        case Condition::OPERATOR_EQUAL:
            return content == myContent;
            break;
        case Condition::OPERATOR_NOT_EQUAL:
            return content != myContent;
            break;
        case Condition::OPERATOR_LESS:
            return content < myContent;
            break;
        case Condition::OPERATOR_MORE:
            return content > myContent;
            break;
        case Condition::OPERATOR_LESS_EQUAL:
            return content <= myContent;
            break;
        case Condition::OPERATOR_MORE_EQUAL:
            return content >= myContent;
            break;
        default:
            return true;
            break;
    }
}

bool Condition::TestRight(string content)
{
    string myContent = value;
    switch (operate)
    {
        case Condition::OPERATOR_EQUAL:
            return content == myContent;
            break;
        case Condition::OPERATOR_NOT_EQUAL:
            return content != myContent;
            break;
        case Condition::OPERATOR_LESS:
            return content < myContent;
            break;
        case Condition::OPERATOR_MORE:
            return content > myContent;
            break;
        case Condition::OPERATOR_LESS_EQUAL:
            return content <= myContent;
            break;
        case Condition::OPERATOR_MORE_EQUAL:
            return content >= myContent;
            break;
        default:
            return true;
            break;
    }
}

Condition::Condition(string a,string v,int o) {
    attributeName = a;
    value = v;
    operate = o;
}

