#include <llvm/Pass.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Constants.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>
#include <unordered_map>
#define Assert(a,b) // an empty Assert macro , because this code is copied from
                    // other project

namespace lle{
   class CacheTime: public llvm::ModulePass
   {
      public:
      static char ID;
      CacheTime():ModulePass(ID) {}
      bool runOnModule(llvm::Module& M) override;
      private:
      typedef llvm::Value* (*TemplateFunc)(llvm::Instruction* InsPoint);
      static std::unordered_map<std::string, TemplateFunc> ImplMap;
      llvm::Value* implyTemplate(llvm::CallInst* Template) const;
   };
};

using namespace llvm;
char lle::CacheTime::ID = 0;
static RegisterPass<lle::CacheTime> X("CacheTime", "CacheTime");
static std::string FunctyStr = "";

#define REPEAT 900000
bool lle::CacheTime::runOnModule(Module &M)
{
    Function *F;
   /*Function* F = M.getFunction("timing");
   Assert(F, "unable find @timing function");
   F->addFnAttr(Attribute::AlwaysInline);

   F = M.getFunction("timing_err");
   Assert(F, "unable find @timing_err function");
   F->addFnAttr(Attribute::AlwaysInline);*/

   if((F = M.getFunction("inst_template")))
   {
	   for(auto Ite = F->user_begin(), E = F->user_end(); Ite!=E; ++Ite){
		  CallInst* Template = dyn_cast<CallInst>(*Ite);
		  Value* R = implyTemplate(Template);
		  Template->replaceAllUsesWith(R);
		  Template->eraseFromParent();
	   }
   }
   else 
      Assert(F, "can not find @inst_template");  
   return true;
}

Value* lle::CacheTime::implyTemplate(CallInst *Template) const
{
   Assert(Template,"");
   ConstantExpr* Arg0 = dyn_cast<ConstantExpr>(Template->getArgOperand(0));
   Assert(Arg0,"");
   GlobalVariable* Global = dyn_cast<GlobalVariable>(Arg0->getOperand(0));
   Assert(Global,"");
   ConstantDataArray* Const = dyn_cast<ConstantDataArray>(Global->getInitializer());
   Assert(Const,"");
   StringRef Selector = Const->getAsCString();

   FunctyStr = Selector.str();
   auto Found = ImplMap.find(Selector.str());
   //AssertRuntime(Found != ImplMap.end(), "unknow template keyword: "<<Selector);

   return Found->second(Template);
}


static Value* load1(Instruction* InsPoint)
{
   CallInst* Template = dyn_cast<CallInst>(InsPoint);
   Value* var = Template->getArgOperand(1);
   LoadInst* l;

	for(int i=0;i<REPEAT;++i)
	{
		l = new LoadInst(var,"",InsPoint);
        l->setVolatile(true);
	}	
	return l;
}

static Value* load2(Instruction* InsPoint)
{
   CallInst* Template = dyn_cast<CallInst>(InsPoint);
   Value* var1 = Template->getArgOperand(1);
   Value* var2 = Template->getArgOperand(2);
   LoadInst* l;

	for(int i=0;i<REPEAT/2;++i)
	{
		l = new LoadInst(var1,"",InsPoint);
        l->setVolatile(true);
        l = new LoadInst(var2,"",InsPoint);
        l->setVolatile(true);
	}	
	return l;
}

static Value* load3(Instruction* InsPoint)
{
   CallInst* Template = dyn_cast<CallInst>(InsPoint);
   Value* var1 = Template->getArgOperand(1);
   Value* var2 = Template->getArgOperand(2);
   Value* var3 = Template->getArgOperand(3);
   LoadInst* l;

	for(int i=0;i<REPEAT/3;++i)
	{
		l = new LoadInst(var1,"",InsPoint);
        l->setVolatile(true);
        l = new LoadInst(var2,"",InsPoint);
        l->setVolatile(true);
        l = new LoadInst(var3,"",InsPoint);
        l->setVolatile(true);
	}	
	return l;
}

static Value* load4(Instruction* InsPoint)
{
   CallInst* Template = dyn_cast<CallInst>(InsPoint);
   Value* var1 = Template->getArgOperand(1);
   Value* var2 = Template->getArgOperand(2);
   Value* var3 = Template->getArgOperand(3);
   Value* var4 = Template->getArgOperand(4);
   LoadInst* l;

	for(int i=0;i<REPEAT/4;++i)
	{
		l = new LoadInst(var1,"",InsPoint);
        l->setVolatile(true);
        l = new LoadInst(var2,"",InsPoint);
        l->setVolatile(true);
        l = new LoadInst(var3,"",InsPoint);
        l->setVolatile(true);
        l = new LoadInst(var4,"",InsPoint);
        l->setVolatile(true);
	}	
	return l;
}

std::unordered_map<std::string, lle::CacheTime::TemplateFunc> 
lle::CacheTime::ImplMap = {
   {"load1",    load1},
   {"load2",    load2},
   {"load3",    load3},
   {"load4",    load4}
};
