// Nodes generator
// define NODE_NAME, NODE_XML_START and NODE_XML_END and include this file

class NODE_NAME: public Node {
	private:
	public:
		NODE_NAME(string token):
		Node(token)
		{}
		
		string XMLStart() const {
			string xml = NODE_XML_START;
			return xml;
		}
		
		string XMLEnd() const {
			string xml = NODE_XML_END;
			return xml;
		}
};

