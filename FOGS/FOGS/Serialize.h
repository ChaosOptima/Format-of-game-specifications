#pragma once

#include <functional>
#include <map>
#include "FOGS.h"

namespace FOGS
{
	class NoCopyConstruct
	{
		NoCopyConstruct(const NoCopyConstruct&) = delete;
		NoCopyConstruct(const NoCopyConstruct&&) = delete;
	public:
		NoCopyConstruct(char){};
	};
	

	template<class Container>
	class Serializable
	{
	protected:
		typedef std::function<void(const std::string&, Container&)> Func;

		struct SerializerPair
		{
			Func Serializer;
			Func Deserializer;
		};

		Container* ContainerInf = 0;

		Serializable() = default;
		Serializable(const Serializable&){};
		Serializable& operator=(const Serializable&){ return *this; };
		~Serializable() = default;

		char Add(std::string _key, Func _serializer, Func _deserializer)
		{
			auto& lv_Pair = m_Serializers[_key];
			lv_Pair.Serializer = _serializer;
			lv_Pair.Deserializer = _deserializer;

			return 0;
		}

	public:
		virtual void Serialize(Container& _cont)
		{
			for (auto& lv_Ser : m_Serializers)
				lv_Ser.second.Serializer(lv_Ser.first, _cont);
		}

		virtual void Deserialize(Container& _cont)
		{
			for (auto& lv_Ser : m_Serializers)
				lv_Ser.second.Deserializer(lv_Ser.first, _cont);
		}

	private:

		std::map<std::string, SerializerPair> m_Serializers;
	};

	template<>
	class Serializable < Node >
	{
	protected:
		typedef std::function<void(const std::string&, Node&)> Func;

		struct SerializerPair
		{
			Func Serializer;
			Func Deserializer;
		};

		Node* ContainerInf = 0;

		Serializable() = default;
		Serializable(const Serializable&){};
		Serializable& operator=(const Serializable&){ return *this; };
		~Serializable() = default;

		char Add(std::string _key, Func _serializer, Func _deserializer)
		{
			auto& lv_Pair = m_Serializers[_key];
			lv_Pair.Serializer = _serializer;
			lv_Pair.Deserializer = _deserializer;

			return 0;
		}

	public:
		virtual void Serialize(Node& _cont)
		{
			for (auto& lv_Ser : m_Serializers)
				lv_Ser.second.Serializer(lv_Ser.first, _cont);
		}

		virtual void Deserialize(Node& _cont)
		{
			auto lv_Map = _cont.MapChilds();
			for (auto& lv_Ser : m_Serializers)
				lv_Ser.second.Deserializer(lv_Ser.first, *lv_Map[lv_Ser.first].begin());
		}

	private:

		std::map<std::string, SerializerPair> m_Serializers;
	};






	template<class T>
	void Serialize(const std::string& _key, T* _val, Node& _node, ...)
	{
		_node.AppendChild().Name(_key).Value() = *_val;
	}

	template<class T>
	void Deserialize(const std::string& _key, T* _val, Node& _node, ...)
	{
		*_val = _node.Value().Item();
	}

	template<class T>
	void Serialize(const std::string& _key, T* _val, Node& _node, Serializable<Node>*)
	{
		_val->Serialize(_node.AppendChild().Name(_key));
	}

	template<class T>
	void Deserialize(const std::string& _key, T* _val, Node& _node, Serializable<Node>*)
	{
		_val->Deserialize(_node);
	}



	template<class T>
	void Serialize(const std::string& _key, std::map<std::string, T>* _val, Node& _node, ...)
	{
		auto lv_Node = _node.AppendChild().Name(_key);
		for (auto lv_Val : *_val)
			Serialize(lv_Val.first, &lv_Val.second, lv_Node, (T*)0);
	}

	template<class T>
	void Deserialize(const std::string& _key, std::map<std::string, T>* _val, Node& _node, ...)
	{
		_val->clear();

		for (auto lv_Val : _node.Childs())
			Deserialize(lv_Val.Name(), &(*_val)[lv_Val.Name()], lv_Val, (T*)0);
	}

	template<class T>
	void Serialize(const std::string& _key, std::list<T>* _val, Node& _node, ...)
	{
		auto lv_Node = _node.AppendChild().Name(_key);
		SerializeList(_key, _val, lv_Node, (T*)0);
	}

	template<class T>
	void Deserialize(const std::string& _key, std::list<T>* _val, Node& _node, ...)
	{
		const char lv_Lable[sizeof(T)] = { 0 };
		DeserializeList(_key, _val, _node, (T*)0);
	}


	template<class T>
	void SerializeList(const std::string& _key, std::list<T>* _val, Node& _node, ...)
	{
		auto lv_Node = _node.AppendChild().Name(_key);
		for (auto lv_Val : *_val)
			lv_Node.Value() += _val;
		

	}

	template<class T>
	void DeserializeList(const std::string& _key, std::list<T>* _val, Node& _node, ...)
	{
		_val->clear();

		for (auto& lv_Val : _node.Value().Items())
			_val->push_back(lv_Val);
	}

	template<class T>
	void SerializeList(const std::string& _key, std::list<T>* _val, Node& _node, Serializable<Node>*)
	{
		int i = 0;
		for (auto& lv_Val : *_val)
			Serialize(std::to_string(i++), &lv_Val, _node, &lv_Val);
	}

	template<class T>
	void DeserializeList(const std::string& _key, std::list<T>* _val, Node& _node, Serializable<Node>*)
	{
		_val->clear();

		for (unsigned int i = 0; i < _node.ChildsCount(); i++)
		{
			T lv_Tmp = T();
			Deserialize("", &lv_Tmp, _node[std::to_string(i)], &lv_Tmp);
			_val->push_back(lv_Tmp);
		}
	}

	template<class T>
	void Serialize(const std::string& _key, std::vector<T>* _val, Node& _node, ...)
	{
		auto lv_Node = _node.AppendChild().Name(_key);
		SerializeVector(_key, _val, lv_Node, (T)0);
	}

	template<class T>
	void Deserialize(const std::string& _key, std::vector<T>* _val, Node& _node, ...)
	{
		const char lv_Lable[sizeof(T)] = { 0 };
		DeserializeVector(_key, _val, _node, (T*)0);
	}


	template<class T>
	void SerializeVector(const std::string& _key, std::vector<T>* _val, Node& _node, ...)
	{
		auto lv_Node = _node.AppendChild().Name(_key);

		for (auto& lv_Val : *_val)
			lv_Node.Value() += _val;
	}

	template<class T>
	void DeserializeVector(const std::string& _key, std::vector<T>* _val, Node& _node, ...)
	{
		_val->clear();
		_val->resize(_node.Value().ItemsCount());

		for (auto& lv_Val : _node.Value().Items())
			_val->push_back(lv_Val);
	}



	template<class T>
	void SerializeVector(const std::string& _key, std::vector<T>* _val, Node& _node, Serializable<Node>*)
	{
		int i = 0;
		for (auto lv_Val : *_val)
			Serialize(std::to_string(i++), &lv_Val, _node, &lv_Val);
	}

	template<class T>
	void DeserializeVector(const std::string& _key, std::vector<T>* _val, Node& _node, Serializable<Node>*)
	{
		_val->clear();
		_val->resize(_node.ChildsCount());

		for (int i = 0; i < _node.ChildsCount(); i++)
			Deserialize("", &_val[i], _node[std::to_string(i)], &_val[i]);
		
	}

}




#define UNNAMED_IMPL(x, y) UNNAMED_##x##_##y
#define UNNAMED_DECL(x, y) UNNAMED_IMPL(x, y)
#define UNNAMED UNNAMED_DECL(__LINE__ , __COUNTER__)

#define serialize(x) NoCopyConstruct UNNAMED = Add																	\
(																										\
	#x,																									\
	[this](const string& _key, ClearType<decltype(ContainerInf)>::Type& _cont)							\
	{																									\
		::Serialize(_key, &x, _cont, (ClearType<decltype(x)>::Type*)0);									\
	},																									\
																										\
	[this](const string& _key, ClearType<decltype(ContainerInf)>::Type& _cont) mutable					\
	{																									\
		::Deserialize(_key, &x, _cont, (ClearType<decltype(x)>::Type*)0);								\
	}																									\
)