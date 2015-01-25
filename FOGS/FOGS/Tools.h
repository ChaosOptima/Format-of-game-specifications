#pragma once

namespace FOGS
{
	template<class T>
	struct ClearType
	{
		typedef T Type;
	};

	template<class T>
	struct ClearType < T* >
	{
		typedef T Type;
	};

	template<class T>
	struct ClearType < T& >
	{
		typedef T Type;
	};

	template<class Lambda, class...Lambdas>
	class LambdaOverload : public Lambda, public LambdaOverload < Lambdas... >
	{
	public:
		LambdaOverload(Lambda _val1, Lambdas..._val2) : Lambda(_val1), LambdaOverload < Lambdas... >(_val2...){}
		using Lambda::operator();
		using LambdaOverload < Lambdas... >::operator();
	};

	template<class Lambda>
	class LambdaOverload <Lambda> : public Lambda
	{
	public:
		LambdaOverload(Lambda _val1) : Lambda(_val1){}
		using Lambda::operator();
	};

	template<class...Lambdas>
	LambdaOverload<Lambdas...> CreateOverloaded(Lambdas..._funcs)
	{
		return LambdaOverload<Lambdas...>(_funcs...);
	}
}