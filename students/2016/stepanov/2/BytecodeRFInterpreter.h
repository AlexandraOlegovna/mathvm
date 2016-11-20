//
// Created by user on 11/12/16.
//

#ifndef VM_BYTECODE_PRINTER_INTERPRETER_H
#define VM_BYTECODE_PRINTER_INTERPRETER_H

#include "../../../../include/mathvm.h"
#include "../../../../include/visitors.h"

namespace mathvm {
    class StackItem {
    public:
        union {
            int64_t intValue;
            double doubleValue;
            uint16_t stringIdValue;
        } value;
        VarType type;
        StackItem(int64_t arg) {
            value.intValue = arg;
            type = VT_INT;
        }

        StackItem(double arg) {
            value.doubleValue = arg;
            type = VT_DOUBLE;
        }

        StackItem(uint16_t arg) {
            value.stringIdValue = arg;
            type = VT_STRING;
        }

        inline double getDouble() {
            assert (type == VT_DOUBLE);
            return value.doubleValue;
        }

        inline int64_t getInt() {
            assert (type == VT_INT);
            return value.intValue;
        }

        inline  uint16_t getStringId() {
            assert (type == VT_STRING);
            return value.stringIdValue;
        }
    };

    static StackItem STACK_EMPTY = StackItem((int64_t)0);

    class InterScope {
    private:
        Status *status = Status::Ok();
        Bytecode *bytecode;
        BytecodeFunction *bf;
    public:
        uint32_t IP = 0;
        std::vector<StackItem> variables;
        InterScope * parent;
        Status *getStatus() const;

        InterScope(BytecodeFunction *bf, InterScope * parent = nullptr);

        Instruction next();

        inline uint16_t nextUint16t();

        inline void skip2xUint16t();

        inline void skipUint16t();

        inline int64_t nextInt();

        inline double nextDouble();

        inline void jump();

        StackItem* variableLookup(uint16_t scope, uint16_t variable){
            if (bf->scopeId() == scope){
                return &(variables[variable]);
            }
            else if (parent != nullptr){
                return parent->variableLookup(scope, variable);
            }
            return nullptr;
        }
   };

    class BytecodeRFInterpreterCode : public Code {
    private:
        std::vector<StackItem>* variablesPtr;
        std::vector<StackItem> stack;
        InterScope *is;

        bool evaluateThis(Instruction instr);
        uint16_t emptyString();

    public:
        BytecodeRFInterpreterCode() {
            stack.reserve(100);
        }

        virtual Status *execute(vector<Var *> &vars) override {
            BytecodeFunction *topFunction = (BytecodeFunction *) functionByName(AstFunction::top_name);
            is = new InterScope(topFunction);
            variablesPtr = &(is->variables);

            Instruction nextInstruction;
            while ((nextInstruction = is->next()) != BC_STOP) {
                if (!evaluateThis(nextInstruction)) {
                    break;
                }
            }
            return is ? is->getStatus() : Status::Ok();
        }
    };
}

#endif //VM_BYTECODE_PRINTER_INTERPRETER_H
