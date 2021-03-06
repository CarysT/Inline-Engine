#pragma once

#include <BaseLibrary/Graph/Port.hpp>
#include <BaseLibrary/Graph/SerializableNode.hpp>
#include <BaseLibrary/UniqueIdGenerator.hpp>
#include <GraphicsEngine/Resources/IMaterialShader.hpp>

#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>

#ifdef _MSC_VER // disable lemon warnings
#pragma warning(push)
#pragma warning(disable : 4267)
#endif

#include <lemon/list_graph.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#undef GetClassName


namespace inl::gxeng {


class ShaderManager;
class MaterialShader;
class MaterialShaderInput;
class MaterialShaderOutput;


struct FunctionParameter {
	bool out;
	std::string type;
	std::string name;
	std::string defaultValue;
};

struct FunctionSignature {
	std::string returnType;
	std::vector<FunctionParameter> parameters;
};


class MaterialShaderOutput : public IMaterialShaderOutput {
public:
	MaterialShaderOutput() = default;
	MaterialShaderOutput(const MaterialShader& parent);
	MaterialShaderOutput(const MaterialShader& parent, std::string name, std::string type, int index);
	~MaterialShaderOutput();

	void Link(IMaterialShaderInput* target) override;
	void Link(MaterialShaderInput* target);
	void UnlinkAll() override;
	void Unlink(IMaterialShaderInput* target) override;
	void Unlink(MaterialShaderInput* target);
	const IMaterialShader* GetParent() const override;

	const std::vector<IMaterialShaderInput*>& GetLinks() const override;

	const std::string& GetType() const override;
	const std::string& GetName() const override;

	const std::string name;
	const std::string type;
	const int index = 0;

private:
	std::vector<IMaterialShaderInput*> m_links;
	const MaterialShader* m_parent = nullptr;
};



class MaterialShaderInput : public IMaterialShaderInput {
	friend class MaterialShaderOutput;

public:
	MaterialShaderInput() = default;
	MaterialShaderInput(const MaterialShader& parent);
	MaterialShaderInput(const MaterialShader& parent, std::string name, std::string type, int index, std::string defaultValue = {});
	~MaterialShaderInput();

	void Link(IMaterialShaderOutput* source) override;
	void Link(MaterialShaderOutput* source);
	void Unlink() override;
	const IMaterialShader* GetParent() const override;

	MaterialShaderOutput* GetLink() const override;

	const std::string& GetType() const override;
	const std::string& GetName() const override;

	void SetDefaultValue(std::string defaultValue) override;
	std::string GetDefaultValue() const override;

	std::string ToString() const override;
	bool IsSet() const override;

	const std::string name; // HLSL variable name of the port.
	const std::string type; // HLSL type of the port.
	const int index = 0; // Index in the HLSL function parameter list.
private:
	std::string m_defaultValue;
	MaterialShaderOutput* m_link = nullptr;
	const MaterialShader* m_parent = nullptr;
};


class MaterialShader : public virtual IMaterialShader {
public:
	MaterialShader(const ShaderManager* shaderManager);

	// Shaders
	virtual const std::string& GetShaderCode() const override = 0;
	virtual size_t GetHash() const override { return std::hash<std::string>()(GetShaderCode()); }

	void SetDisplayName(std::string name) override { m_name = std::move(name); }
	const std::string& GetDisplayName() const override { return m_name; }

	std::string GetClassName() const override { return m_className; }

	// Ports
	size_t GetNumInputs() const override { return m_inputs.size(); }
	size_t GetNumOutputs() const override { return m_outputs.size(); }

	MaterialShaderInput* GetInput(size_t index) override { return &m_inputs[index]; }
	MaterialShaderOutput* GetOutput(size_t index) override { return &m_outputs[index]; }

	const MaterialShaderInput* GetInput(size_t index) const override { return &m_inputs[index]; }
	const MaterialShaderOutput* GetOutput(size_t index) const override { return &m_outputs[index]; }

	const std::string& GetInputName(size_t index) const override;
	const std::string& GetOutputName(size_t index) const override;

	// Hashing
	/// <summary> Returns an ID that is the same for all MaterialShaders with the same shader code.
	///		Does not change throughout program execution. </summary>
	UniqueId GetId() const { return m_id; }

protected:
	// HLSL string processing helpers
	static std::string RemoveComments(std::string code);
	static std::string GetFunctionSignature(const std::string& code, const std::string& functionName);
	static FunctionSignature DissectFunctionSignature(const std::string& signatureString);

	void SetClassName(std::string className) { m_className = std::move(className); }
	void RecalcId();

protected:
	std::vector<MaterialShaderInput> m_inputs;
	std::vector<MaterialShaderOutput> m_outputs;
	const ShaderManager* m_shaderManager;

private:
	std::string m_name;
	std::string m_className;

	UniqueId m_id;
	static std::mutex idGeneratorMtx;
	static UniqueIdGenerator<std::string> idGenerator;
};



class MaterialShaderEquation : public virtual MaterialShader, virtual public IMaterialShaderEquation {
public:
	MaterialShaderEquation(const ShaderManager* shaderManager) : MaterialShader(shaderManager) {}

	const std::string& GetShaderCode() const override;

	void SetSourceFile(const std::string& name) override;
	void SetSourceCode(std::string code) override;

private:
	void CreatePorts(const std::string& code);

private:
	std::string m_sourceCode;
};



class MaterialShaderGraph : virtual public MaterialShader, virtual public IMaterialShaderGraph {
public:
	MaterialShaderGraph(const ShaderManager* shaderManager) : MaterialShader(shaderManager) {}

	const std::string& GetShaderCode() const override;

	void SetGraph(std::vector<std::unique_ptr<IMaterialShader>> nodes) override;
	auto TopologicalSortNodes(const lemon::ListDigraph& depGraph, const lemon::ListDigraph::NodeMap<MaterialShader*>& depMap)
		-> std::pair<std::vector<lemon::ListDigraph::Node>, std::unordered_map<const MaterialShader*, int>>;
	void SetGraph(std::vector<std::unique_ptr<MaterialShader>> nodes);

	/// <summary> Cannot handle nested graphs. </summary>
	void SetGraph(std::string jsonDescription) override;

private:
	// Creates one graph node per shader node, adds arc in graph for any two shader nodes which have their ports linked together.
	static void CalculateDependencyGraph(const std::vector<std::unique_ptr<MaterialShader>>& nodes,
										 lemon::ListDigraph& depGraph,
										 lemon::ListDigraph::NodeMap<MaterialShader*>& depMap);

	// Gets the list of unlinked (free) ports of the node graph.
	static auto GetUnlinkedPorts(const std::vector<std::unique_ptr<MaterialShader>>& nodes)
		-> std::tuple<std::vector<MaterialShaderInput*>, std::vector<MaterialShaderOutput*>>;


	// Creates the parameter list string of the main function using the list of unlinked ports.
	// Parameters are renamed by adding the name of the parent node.
	// Throws an exception if two parameters have the same name, because that's invalid in HLSL.
	static std::string CreateParameterString(const std::vector<MaterialShaderInput*>& inputs,
											 const std::vector<MaterialShaderOutput*>& outputs);

	// Creates input and outputs ports.
	void CreatePorts(const std::vector<MaterialShaderInput*>& inputs,
					 const std::vector<MaterialShaderOutput*>& outputs);

private:
	std::vector<std::unique_ptr<MaterialShader>> m_nodes;
	std::string m_sourceCode;
};


} // namespace inl::gxeng