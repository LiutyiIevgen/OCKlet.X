/*
 * File:   Parameters.h
 * Author: andrey
 *
 * Created on 5 ?????? 2014, 9:44
 * contains methods for can parameters processing
 */

/**
 * @author Kyrylov Andrii
 * @param char* - new parameter name
 * @param char - new parameter type
 * @param char* - parameter value array
 * @param unsigned int - byte count in value array
 * @todo make new parameter in fat structure
 * @return 1 - parameter was added;
 * 0 - there is not enough space
 */
char AddParameter(char*, char, char*, unsigned int);
/**
 * @author KyrylovAndrii
 * @param unsigned int parameter index
 * @todo delete parameter by parameter Index
 * @return 1 - parameter was deleted;
 * 0 - parameter wasn't deleted
 */
char DeleteParameter(unsigned int);
/**
 * @author Liutyi Ievgen
 * @param unsigned int param1 is index of necessary parameter
 * @param char* param2 is bufer for value
 * @todo read value of necessary parameter in bufer
 * @return size of parameter value in bytes
 * @return 0 - there are no parameter with such index
 */
unsigned int ReadParameterValue(unsigned int, char*);
/**
 * @author Liutyi Ievgen
 * @param unsigned int param1 is index of necessary parameter
 * @param char* param2 is bufer for name
 * @todo read name of necessary parameter in bufer
 * @return size of parameter name in bytes
 * @return 0 - there are no parameter with such index
 */
unsigned char ReadParameterName(unsigned int, char*);
/**
 * @author Liutyi Ievgen
 * @param unsigned int param1 is index of necessary parameter
 * @todo read type of necessary parameter
 * @return type of necessary parameter
 * @return -1 => there are no parameter with such index
 */
char ReadParameterType(unsigned int);
/**
 * @author Liutyi Ievgen
 * @param unsigned int param1 is index of necessary parameter
 * @param char* param2 is parameter value array
 * @param unsigned int param3 is byte count in value array
 * @todo change value of necessary parameter
 */
void EditParameterValue(unsigned int, char*, unsigned int);
/**
 * @author Liutyi Ievgen
 * @param unsigned int param1 is index of necessary parameter
 * @param char* param2 is parameter new name
 * @todo change name of necessary parameter
 * @return 1 - name was changed successfully
 * @return 0 - name wasn't changed 
 */
char EditParameterName(unsigned int, char*);
/**
 * @author Liutyi Ievgen
 * @todo find max index among existing parameter's descriptors
 * @return max index
 */
unsigned int FindMaxIndex();
/**
 * @author Liutyi Ievgen
 * @param index of necessary parameter
 * @todo find descriptor address by index
 * @return descriptor address
 * @return 0xFFFF => there are no descriptor with such index
 */
unsigned int FindDescriptorAdrByIndex(unsigned int);