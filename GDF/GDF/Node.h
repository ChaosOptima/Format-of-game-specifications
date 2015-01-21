#pragma once
#include <map>
#include <list>
#include "Value.h"
#include "Attribute.h"

namespace GameDataFormat
{
	class GDF_Context;
	
	struct NodeData
	{
		NodeData(GDF_Context* _context);
		~NodeData();

		void ReadNodes();
		void ReadName();
		void ReadAttributes();
		void NameEnds();
		bool IsUnexpected(char* _char);
		bool NodesReader(char* _char);
		bool AttributesReader(char* _char);
		bool NameReader(char* _char);

		void AppendNode();

		char* m_Name = 0;
		NodeData* m_Sibling = 0;
		NodeData* m_Nodes = 0;
		NodeData* m_Last = 0;
		unsigned int m_NodesSize = 0;

		AttributeData* m_Attributes = 0;
		AttributeData* m_AttLast = 0;
		unsigned int m_AttrSize = 0;

		bool m_NameEnds = false;
		bool ContextHolder = true;
		ValueData* m_Value = 0;
		GDF_Context* m_Context = 0;
		NodeData* m_Parent = 0;

		char* m_End = 0;
	};

	extern Allocator<sizeof(NodeData)> NodeAlloc;
	extern Allocator<sizeof(AttributeData)> AttrAlloc;

	class GDF_Node
	{
	public:
		class Iterator
		{
			NodeData* m_Root;
		public:
			Iterator(NodeData* _root);
			GDF_Node operator*() const;
			Iterator& operator++();
			bool operator!=(const Iterator& _rval);
		};

		Iterator begin();
		Iterator end();

		GDF_Node(NodeData* _data);
		
		std::string Name();
		GDF_Node& Name(const std::string& _val);
		GDF_Node& Name(const char* _val);

		bool IsNull();
		GDF_Node Next();
		GDF_Value Value();

		unsigned int ChildsCount();
		GDF_Node Childe(const std::string& _key);
		GDF_Node AppendChild();
		GDF_Node AppendChild(const char* _name);
		GDF_Node AppendChild(const std::string& _name);
		std::list<GDF_Node> Childs();
		std::list<GDF_Node> Childs(const char* _name);
		std::list<GDF_Node> Childs(const std::string& _name);
		std::map<std::string, std::list<GDF_Node>> MapChilds();

		unsigned int AttributesCount();
		GDF_Attribute Attribute(const std::string& _key);
		GDF_Attribute AppendAttribute();
		GDF_Attribute AppendAttribute(const char* _name);
		GDF_Attribute AppendAttribute(const std::string& _name);
		std::list<GDF_Attribute> Attributes();
		std::list<GDF_Attribute> Attributes(const char* _name);
		std::list<GDF_Attribute> Attributes(const std::string& _name);
		std::map<std::string, std::list<GDF_Attribute>> MapAttributes();

		operator bool();
		GDF_Node operator[](const std::string& _key);
		GDF_Node operator[](const char* _key);
		GDF_Attribute operator()(const std::string& _key);	

	private:
		NodeData* m_Data = 0;
	};

	GDF_Node::Iterator begin(GDF_Node& _val);
	GDF_Node::Iterator end(GDF_Node& _val);
}