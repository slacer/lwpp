#pragma once
#include <lwextrend.h>
#include "lwpp/plugin_handler.h"

namespace lwpp {
	class ExternalRendererHandler : public InstanceHandler, public ItemHandler/*, public LWPanelInterface*/ {
	protected:
		LWItemID Context;

		LWImageUtil* imageUtil;
		LWSceneInfo* sceneInfo;
		LWItemInfo* itemInfo;
		LWObjectInfo* objectInfo;
		LWCameraInfo* cameraInfo;

	public:
		ExternalRendererHandler(void* g, void* context, LWError* err) : InstanceHandler(g, context, err, LWEXTRENDERER_HCLASS)
		{
			Context = LWItemID(context);
			sceneInfo = (LWSceneInfo*)lwpp::SuperGlobal(LWSCENEINFO_GLOBAL, GFUSE_TRANSIENT);
			imageUtil = (LWImageUtil*)lwpp::SuperGlobal(LWIMAGEUTIL_GLOBAL, GFUSE_TRANSIENT);
			itemInfo = (LWItemInfo*)lwpp::SuperGlobal(LWITEMINFO_GLOBAL, GFUSE_TRANSIENT);
			objectInfo = (LWObjectInfo*)lwpp::SuperGlobal(LWOBJECTINFO_GLOBAL, GFUSE_TRANSIENT);
			cameraInfo = (LWCameraInfo*)lwpp::SuperGlobal(LWCAMERAINFO_GLOBAL, GFUSE_TRANSIENT);
		}
		virtual ~ExternalRendererHandler() { ; }
		LWInstance create(void* priv, LWItemID id, LWError* err) {

		}
		virtual int Render() {
			return 1;
		}
	
	};


	template <class T, int minVersion>
	class ExternalRendererAdaptor : public InstanceAdaptor<T>, public ItemAdaptor<T>{
	public:
		ExternalRendererAdaptor(const char* name, ServerTagInfo* tags) 
		{
			LWServer::AddPlugin(LWEXTRENDERER_HCLASS, name, Activate, tags);
		}
		static int Activate(int version, GlobalFunc* global, LWInstance inst, void* serverData) {

			if (version < minVersion) return AFUNC_BADVERSION;

			try
			{
				UNUSED(serverData);
				UNUSED(version);
				lwpp::SetSuperGlobal(global);

				LWExtRendererHandler* plugin = static_cast<LWExtRendererHandler*>(inst);
				lwpp::InstanceAdaptor<T>::Activate(plugin->inst);
				lwpp::ItemAdaptor<T>::Activate(plugin->item);
				plugin->render = ExternalRendererAdaptor::Render;
				plugin->options = ExternalRendererAdaptor::Options;

				return AFUNC_OK;
			}
			catch (const std::exception& e)
			{
				lwpp::LWMessage::Error("An exception occured in ExtRenderer::Activate():", e.what());
				return AFUNC_BADAPP;
			}

		}

		static int Render(LWInstance instance, int first_frame, int last_frame, int frame_step, EXTRENDERERIMAGE* render_image, void* user, int render_mode) {
			try {
				T* plugin = static_cast<T*>(instance);
				plugin->render(instance, first_frame, last_frame, frame_step, render_image, user, render_mode);
			}
			catch (std::exception& e)
			{
				const char* msg = e.what();
			}

			return 1;
		}

		static int Options(LWInstance instance) {
			try {
				T* plugin = static_cast<T*>(instance);
				plugin->options(instance);
			}
			catch (std::exception& e)
			{
				const char* msg = e.what();
			}

			return 1;
		}
	};

	IMPLEMENT_LWPANELADAPTOR(ExternalRenderer, LWEXTRENDERER_VERSION);
	IMPLEMENT_LWPANELHANDLER(ExternalRenderer);
}