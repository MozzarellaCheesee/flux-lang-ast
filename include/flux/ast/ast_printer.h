#pragma once
#include "flux/ast/ast_visitor.h"
#include "flux/ast/decl.h"
#include "stmt.h"
#include <iostream>
#include <string>

namespace flux {

    class ASTPrinter : public ASTVisitor {
    private:
        std::ostream& out_;
        std::string prefix_;
        bool is_last_ = true;

        void printLine(const std::string& label) {
            out_ << prefix_ << (is_last_ ? "+-- " : "|-- ") << label << "\n";
        }

        struct Indent {
            ASTPrinter& p;
            std::string saved_prefix;
            bool saved_last;
            Indent(ASTPrinter& p, bool is_last_child) 
                : p(p), saved_prefix(p.prefix_), saved_last(p.is_last_) {
                p.prefix_ += (p.is_last_ ? "    " : "|   ");
                p.is_last_ = is_last_child;
            }
            ~Indent() {
                p.prefix_ = saved_prefix;
                p.is_last_ = saved_last;
            }
        };

    public:
        explicit ASTPrinter(std::ostream& out = std::cout) : out_(out) {}


        // ── Program ─────────────────────────────────────────

        void visit(ProgramNode& node) override {
            out_ << "ProgramNode\n";
            for (size_t i = 0; i < node.decls.size(); ++i) {
                is_last_ = (i + 1 == node.decls.size());
                node.decls[i]->accept(*this);
            }
        }

        // ── Types ────────────────────────────────────────
        void visit(PrimitiveType& node) override {
            printLine("PrimitiveType { name: \"" + node.name + "\" }");
        }

        void visit(RefType& node) override {
            printLine("RefType");
            Indent ind(*this, true);
            if (node.inner) node.inner->accept(*this);
        }

        void visit(UnitType& node) override {
            printLine("UnitType");
        }

        void visit(ArrayType& node) override {
            printLine("ArrayType");
            {
                Indent ind(*this, false);
                printLine("elem_type:");
                if (node.element) {
                    Indent ind2(*this, true);
                    node.element->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("size expr:");
                if (node.size_expr) {
                    Indent ind2(*this, true);
                    node.size_expr->accept(*this);
                }
            }
        }

        void visit(GenericType& node) override {
            printLine("GenericType { name: \"" + node.name + "\" }");
            Indent ind(*this, true);
            for (size_t i = 0; i < node.args.size(); ++i) {
                Indent ind_arg(*this, i + 1 == node.args.size());
                node.args[i]->accept(*this);
            }
        }

        void visit(SliceType& node) override {
            printLine("SliceType");
            Indent ind(*this, true);
            if (node.element) node.element->accept(*this);
        }

        // ── Declarations ─────────────────────────────────

        void visit(FuncDecl& node) override {
            printLine(
                "FuncDecl { is_pub: " + std::string(node.is_pub ? "true" : "false") +
                ", has_self: " + std::string(node.has_self ? "true" : "false") +
                ", name: \"" + node.name + "\" }"
            );

            {
                Indent ind(*this, false);
                printLine("params:");
                for (size_t i = 0; i < node.params.size(); ++i) {
                    Indent ind_p(*this, i + 1 == node.params.size());
                    node.params[i]->accept(*this);
                }
            }
            {
                Indent ind(*this, false);
                printLine("return_type:");
                if (node.return_type) {
                    Indent ind2(*this, true);
                    node.return_type->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("body:");
                if (node.body) {
                    Indent ind2(*this, true);
                    node.body->accept(*this);
                }
            }
        }

        void visit(VarDecl& node) override {
            printLine("VarDecl { name: \"" + node.name + "\" }");
            {
                Indent ind(*this, false);
                printLine("type:");
                if (node.type) {
                    Indent ind2(*this, true);
                    node.type->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("init:");
                if (node.init) {
                    Indent ind2(*this, true);
                    node.init->accept(*this);
                }
            }
        }

        void visit(ParamDecl& node) override {
            printLine("ParamDecl { name: \"" + node.name + "\" }");
            Indent ind(*this, true);
            if (node.type) node.type->accept(*this);
        }

        void visit(FieldDecl& node) override {
            printLine("FieldDecl { name: \"" + node.name + "\" }");
            Indent ind(*this, true);
            if (node.type) node.type->accept(*this);
        }

        void visit(StructDecl& node) override {
            printLine("StructDecl { name: \"" + node.name + "\" }");
            {
                Indent ind(*this, true);
                printLine("fields:");
                for (size_t i = 0; i < node.fields.size(); ++i) {
                    Indent ind_f(*this, i + 1 == node.fields.size());
                    node.fields[i]->accept(*this);
                }
            }
        }

        void visit(ImplDecl& node) override {
            printLine("ImplDecl");
            // {
            //     Indent ind(*this, false);
            //     printLine("target type:");
            //     if (node.target_type) {
            //         Indent ind2(*this, true);
            //         node.target_type->accept(*this);
            //     }
            // }
            {
                Indent ind(*this, true);
                printLine("methods:");
                for (size_t i = 0; i < node.methods.size(); ++i) {
                    Indent ind_m(*this, i + 1 == node.methods.size());
                    node.methods[i]->accept(*this);
                }
            }
        }

        void visit(ClassDecl& node) override {
            printLine("ClassDecl { name: \"" + node.name + "\" }");
            {
                Indent ind(*this, false);
                printLine("fields:");
                for (size_t i = 0; i < node.fields.size(); ++i) {
                    Indent ind_f(*this, i + 1 == node.fields.size());
                    node.fields[i]->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("methods:");
                for (size_t i = 0; i < node.methods.size(); ++i) {
                    Indent ind_m(*this, i + 1 == node.methods.size());
                    node.methods[i]->accept(*this);
                }
            }
        }

        // ── Statements ──────────────────────────────────────
        void visit(BlockStmt& node) override {
            printLine("BlockStmt");
            Indent ind(*this, true);
            for (size_t i = 0; i < node.stmts.size(); ++i) {
                Indent ind_s(*this, i + 1 == node.stmts.size());
                node.stmts[i]->accept(*this);
            }
        }

        void visit(ReturnStmt& node) override {
            printLine("ReturnStmt");
            Indent ind(*this, true);
            if (node.value) node.value->accept(*this);
        }

        void visit(ExprStmt& node) override {
            printLine("ExprStmt");
            Indent ind(*this, true);
            if (node.expr) node.expr->accept(*this);
        }

        void visit(IfStmt& node) override {
            printLine("IfStmt");
            {
                Indent ind(*this, false);
                printLine("cond:");
                if (node.cond) {
                    Indent ind2(*this, true);
                    node.cond->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("else:");
                if (node.else_branch) {
                    Indent ind2(*this, true);
                    node.else_branch->accept(*this);
                }
            }
        }

        void visit(ForStmt& node) override {
            printLine("ForStmt");
            {
                Indent ind(*this, false);
                printLine("init:");
                if (node.init) {
                    Indent ind2(*this, true);
                    node.init->accept(*this);
                }
            }
            {
                Indent ind(*this, false);
                printLine("cond:");
                if (node.cond) {
                    Indent ind2(*this, true);
                    node.cond->accept(*this);
                }
            }
            {
                Indent ind(*this, false);
                printLine("step:");
                if (node.step) {
                    Indent ind2(*this, true);
                    node.step->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("body:");
                if (node.body) {
                    Indent ind2(*this, true);
                    node.body->accept(*this);
                }
            }
        }

        void visit(WhileStmt& node) override {
            printLine("WhileStmt");
            {
                Indent ind(*this, false);
                printLine("cond:");
                if (node.cond) {
                    Indent ind2(*this, true);
                    node.cond->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("body:");
                if (node.body) {
                    Indent ind2(*this, true);
                    node.body->accept(*this);
                }
            }
        }

        void visit(ContinueStmt& /*node*/) override {
            printLine("ContinueStmt");
        }

        void visit(BreakStmt& /*node*/) override {
            printLine("BreakStmt");
        }

        void visit(IdentifierPattern& node) override {
            printLine("IdentifierPattern \"" + node.name + "\"");
        }

        // ── Expressions ─────────────────────────────────────

        void visit(IntLiteral& node) override {
            printLine("IntLiteral { value: " + std::to_string(node.value) + " }");
        }

        void visit(FloatLiteral& node) override {
            printLine("FloatLiteral { value: " + std::to_string(node.value) + " }");
        }

        void visit(BoolLiteral& node) override {
            printLine("BoolLiteral { value: " + std::string(node.value ? "true" : "false") + " }");
        }

        void visit(StrLiteral& node) override {
            printLine("StrLiteral { value: \"" + node.value + "\" }");
        }

        void visit(ArrayLiteral& node) override {
            printLine("ArrayLiteral");
            Indent ind(*this, true);
            for (size_t i = 0; i < node.elements.size(); ++i) {
                Indent ind_e(*this, i + 1 == node.elements.size());
                node.elements[i]->accept(*this);
            }
        }

        void visit(IdentExpr& node) override {
            printLine("IdentExpr { name: \"" + node.name + "\" }");
        }

        void visit(BinaryExpr& node) override {
            printLine("BinaryExpr { op: \"" + node.op + "\" }");
            {
                Indent ind(*this, false);
                printLine("lhs:");
                Indent ind2(*this, true);
                node.lhs->accept(*this);
            }
            {
                Indent ind(*this, true);
                printLine("rhs:");
                Indent ind2(*this, true);
                node.rhs->accept(*this);
            }
        }

        void visit(UnaryExpr& node) override {
            printLine("BinaryExpr { op: \"" + std::to_string(static_cast<int>(node.op)) + "\" }");
            Indent ind(*this, true);
            if (node.operand) node.operand->accept(*this);
        }

        void visit(CallExpr& node) override {
            printLine("CallExpr { callee: \"" + node.callee + "\" }");
            Indent ind(*this, true);
            for (size_t i = 0; i < node.args.size(); ++i) {
                Indent ind_a(*this, i + 1 == node.args.size());
                node.args[i]->accept(*this);
            }
        }

        void visit(MethodCallExpr& node) override {
            printLine("MethodCallExpr { method: \"" + node.method + "\" }");
            {
                Indent ind(*this, false);
                printLine("receiver:");
                if (node.receiver) {
                    Indent ind2(*this, true);
                    node.receiver->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("args:");
                for (size_t i = 0; i < node.args.size(); ++i) {
                    Indent ind_a(*this, i + 1 == node.args.size());
                    node.args[i]->accept(*this);
                }
            }
        }

        void visit(IndexExpr& node) override {
            printLine("IndexExpr");
            {
                Indent ind(*this, false);
                printLine("base:");
                Indent ind2(*this, true);
                node.array->accept(*this);
            }
            {
                Indent ind(*this, true);
                printLine("index:");
                Indent ind2(*this, true);
                node.index->accept(*this);
            }
        }

        void visit(AssignExpr& node) override {
            printLine("AssignExpr");
            {
                Indent ind(*this, false);
                printLine("target:");
                Indent ind2(*this, true);
                node.target->accept(*this);
            }
            {
                Indent ind(*this, true);
                printLine("value:");
                Indent ind2(*this, true);
                node.value->accept(*this);
            }
        }

        void visit(ImplicitCastExpr& node) override {
            printLine("ImplicitCastExpr");
            {
                Indent ind(*this, false);
                printLine("target_type:");
                if (node.target_type) {
                    Indent ind2(*this, true);
                    node.target_type->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("inner:");
                if (node.inner) {
                    Indent ind2(*this, true);
                    node.inner->accept(*this);
                }
            }
        }

        void visit(MatchExpr& node) override {
            printLine("MatchExpr");
            {
                Indent ind(*this, false);
                printLine("subject:");
                if (node.subject) {
                    Indent ind2(*this, true);
                    node.subject->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("arms:");
                for (size_t i = 0; i < node.arms.size(); ++i) {
                    Indent ind_a(*this, i + 1 == node.arms.size());
                    node.arms[i]->accept(*this);
                }
            }
        }

        void visit(MatchArm& node) override {
            printLine("MatchArm");
            {
                Indent ind(*this, false);
                printLine("pattern:");
                if (node.pattern) {
                    Indent ind2(*this, true);
                    // PatternNode, если есть accept/visitor
                    node.pattern->accept(*this);
                }
            }
            {
                Indent ind(*this, true);
                printLine("expr:");
                if (node.body) {
                    Indent ind2(*this, true);
                    node.body->accept(*this);
                }
            }
        }

        void visit(FieldAccessExpr& node) override {
            printLine("FieldAccessExpr { field: \"" + node.field + "\" }");
            Indent ind(*this, true);
            if (node.object) node.object->accept(*this);
            
        }

        void visit(StructInitExpr& node) override {
            printLine("StructInitExpr { name: \"" + node.type_name + "\" }");
            Indent ind(*this, true);
            // предполагаем: vector<pair<string, Expr*>>
            for (size_t i = 0; i < node.fields.size(); ++i) {
                auto& [fname, fexpr] = node.fields[i];
                bool is_last = (i + 1 == node.fields.size());
                Indent ind_f(*this, is_last);
                printLine("field \"" + fname + "\"");
                if (fexpr) {
                    Indent ind_e(*this, true);
                    fexpr->accept(*this);
                }
            }
        }

        void visit(SelfExpr& /*node*/) override {
            printLine("SelfExpr");
        }

        void visit(WildcardPattern& node) override {
            printLine("WildcardPattern");
        }

        void visit(LiteralPattern& node) override {
            printLine("LiteralPattern");
            Indent ind(*this, true);
            if (node.value) node.value->accept(*this);
        }

        void visit(ConstructorPattern& node) override {
            printLine("ConstructorPattern { class: \"" + node.name + "\" }");
        }

        void visit(IdentPattern& node) override {
            printLine("IdentPattern { name: \"" + node.name + "\" }");
        }
    };

}