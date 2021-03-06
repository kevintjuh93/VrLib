#include "ModelRenderer.h"
#include <vrlib/Model.h>
#include <VrLib/Texture.h>
#include <vrlib/gl/Vertex.h>
#include "Transform.h"
#include "../Node.h"


namespace vrlib
{
	namespace tien
	{
		namespace components
		{
			std::map<std::string, vrlib::Model*> ModelRenderer::cache;

			ModelRenderer::ModelRenderer(const std::string &fileName)
			{
				if (cache.find(fileName) == cache.end())
					cache[fileName] = vrlib::Model::getModel<vrlib::gl::VertexP3N2B2T2T2>(fileName);
				model = cache[fileName];
				renderContext = ModelRenderContext::getInstance();
			}

			ModelRenderer::~ModelRenderer()
			{

			}

			void ModelRenderer::draw()
			{
				components::Transform* t = node->getComponent<Transform>();

				ModelRenderContext* context = dynamic_cast<ModelRenderContext*>(renderContext);
				context->renderShader->use(); //TODO: only call this once!

				model->draw([this, t, &context](const glm::mat4 &modelMatrix)
				{
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::modelMatrix, t->globalTransform * modelMatrix);
					context->renderShader->setUniform(ModelRenderContext::RenderUniform::normalMatrix, glm::transpose(glm::inverse(glm::mat3(t->globalTransform * modelMatrix))));
				},
					[this, &context](const vrlib::Material &material)
				{
					if (material.texture)
					{
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 1.0f);
						material.texture->bind();
						glActiveTexture(GL_TEXTURE1);
						if (material.normalmap)
							material.normalmap->bind();
						else
							context->defaultNormalMap->bind();
						glActiveTexture(GL_TEXTURE0);

					}
					else
					{
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::textureFactor, 0.0f);
						context->renderShader->setUniform(ModelRenderContext::RenderUniform::diffuseColor, material.color.diffuse);
					}
				});
			}

			void ModelRenderer::ModelRenderContext::init()
			{
				renderShader = new vrlib::gl::Shader<RenderUniform>("data/vrlib/tien/shaders/default.vert", "data/vrlib/tien/shaders/default.frag");
				renderShader->bindAttributeLocation("a_position", 0);
				renderShader->bindAttributeLocation("a_normal", 1);
				renderShader->bindAttributeLocation("a_bitangent", 2);
				renderShader->bindAttributeLocation("a_tangent", 3);
				renderShader->bindAttributeLocation("a_texture", 4);
				renderShader->link();
				renderShader->bindFragLocation("fragColor", 0);
				renderShader->bindFragLocation("fragNormal", 1);
				//shader->bindFragLocation("fragColor", 0);
				renderShader->registerUniform(RenderUniform::modelMatrix, "modelMatrix");
				renderShader->registerUniform(RenderUniform::projectionMatrix, "projectionMatrix");
				renderShader->registerUniform(RenderUniform::viewMatrix, "viewMatrix");
				renderShader->registerUniform(RenderUniform::normalMatrix, "normalMatrix");
				renderShader->registerUniform(RenderUniform::s_texture, "s_texture");
				renderShader->registerUniform(RenderUniform::s_normalmap, "s_normalmap");
				renderShader->registerUniform(RenderUniform::diffuseColor, "diffuseColor");
				renderShader->registerUniform(RenderUniform::textureFactor, "textureFactor");
				renderShader->use();
				renderShader->setUniform(RenderUniform::s_texture, 0);
				renderShader->setUniform(RenderUniform::s_normalmap, 1);

				defaultNormalMap = vrlib::Texture::loadCached("data/vrlib/tien/textures/defaultnormalmap.png");

			}

			void ModelRenderer::ModelRenderContext::frameSetup(const glm::mat4 & projectionMatrix, const glm::mat4 & viewMatrix)
			{
				renderShader->use();
				renderShader->setUniform(RenderUniform::projectionMatrix, projectionMatrix);
				renderShader->setUniform(RenderUniform::viewMatrix, viewMatrix);
				renderShader->setUniform(RenderUniform::diffuseColor, glm::vec4(1, 1, 1, 1));
				renderShader->setUniform(RenderUniform::textureFactor, 1.0f);
			}

		}
	}
}