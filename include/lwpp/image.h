/*!
 * @file
 * @brief Wrapper for LWImageUtil
 */


#ifndef LWPP_IMAGE_H
#define LWPP_IMAGE_H
#include <lwpp/global.h>
#include <lwimage.h>
#include <lwnodes.h>
#include <lwpp/wrapper.h>
#include <lwpp/storeable.h>
#include <lwpp/vector3d.h>

#ifdef _DEBUG
#include <lwpp/debug.h>
#endif

namespace lwpp
{
	//! Wrapper for LWImageUtil
	//! @ingroup Entities
	class ImageUtil : public PopUpCallback, protected GlobalBase<LWImageUtil>, public Storeable
	{
	private:
		LWPixmapID pixmap;
		bool doDestroy;
	public:
		ImageUtil(LWPixmapID _pixmap = 0) 
			: pixmap(_pixmap), doDestroy(false)
		{;}
		//! Optional Constructor
		/*!
		 * @param w Width of the pixmap
		 * @param h Height of the pixmap
		 * @param type LWImageType of the pixmap
		 */
		ImageUtil(int w, int h, LWImageType type) 
			: pixmap(0), doDestroy(true)
		{
			create(w, h, type);
		}
		//! Destroys associated pixmap as well
		~ImageUtil()
		{
			if ((pixmap != 0) && doDestroy) globPtr->destroy(pixmap);
		}

		ImageUtil(const ImageUtil &iu)
		{
			pixmap = iu.pixmap;
			doDestroy = iu.doDestroy;
		}
		LWPixmapID getImage() {
			return pixmap;
		}
		ImageUtil &operator=(const ImageUtil &iu)
		{
			if (&iu != this)
			{
				if ((pixmap != 0) && doDestroy) globPtr->destroy(pixmap);
				pixmap = iu.pixmap;
			}
			return *this;
		}

		//! Returns the specific stride size in bytes for a pixel type
		static size_t GetStride(int type);
		//! Returns the specific stride size in bytes for a pixel type
		size_t GetStride()
		{
			int w,h,type;
			getInfo(&w,&h,&type);
			return GetStride(type);
		}
		void setDestroy(bool destroy = true)
		{
			doDestroy = destroy;
		}
		//! check if we have a valid pixmap
		bool isValid()
		{
			return pixmap != 0;
		}
		void create(int w, int h, LWImageType type) 
		{
			pixmap = globPtr->create(w, h, type);
		}
		//! Set a pixel in the pixmap
		/*!
		 * @param x X coordinate
		 * @param y Y coordinate
		 * @param *pix Pointer to the pixel, depends on the LWImageType of the pixmap
		 */
		void setPixel(int x, int y, void *pix)
		{
			globPtr->setPixel(pixmap, x, y, pix);
		}
		//! Get a pixel from the pixmap
		/*!
		 * @param x X coordinate
		 * @param y Y coordinate
		 * @param *pix Pointer for pixel storage, depends on the LWImageType of the pixmap
		 */
		void getPixel(int x, int y, void *pix)
		{
			globPtr->getPixel(pixmap, x, y, pix);
		}
		//! Get information about the current pixmap
		/*!
		 * @param *w Width
		 * @param *h Height
		 * @param *type Type of Bitmap
		 */
		void getInfo(int *w, int *h, int *type)
		{
			globPtr->getInfo(pixmap, w, h, type);
		}
		static void getInfo(LWPixmapID id, int *w, int *h, int *type)
		{
			globPtr->getInfo(id, w, h, type);
		}

		LWPixmapID  resample(int w, int h, int mode )
		{
			return globPtr->resample(pixmap, w, h, mode);
		}

		void setPixelTyped(int x, int y, int type, void *pix )
		{
			globPtr->setPixelTyped(pixmap, x, y, type, pix);
		}
		void getPixelTyped ( int x, int y, int type, void *pix )
		{
			globPtr->getPixelTyped(pixmap, x, y, type, pix);
		}
		int  getIndex8Map( LWPixelRGB24 *map )
		{
			return globPtr->getIndex8Map(pixmap, map);
		}
		int  getAttr ( LWImageParam tag, void* data )
		{
			return globPtr->getAttr(pixmap, tag, data);
		}
		int  getMakerNote ( LWMakerNote tag, void* data )
		{
			return globPtr->getMakerNote(pixmap, tag, data);
		}
		//! Save the current pixmap
		void save(int saver, const std::string name)
		{
			globPtr->save(pixmap, saver, name.c_str());
		}

		//! return the amount of savers
		/*!
		 * @return The number of savers present in LightWave
		 */
		static int saverCount(void)
		{
			return globPtr->saverCount();
		}

		//! return the name of saver n
		/*!
		 * @param n Index of the saver to be queried
		 * @return The name of the save
		 */
		static const char *saverName(int n)
		{
			return globPtr->saverName(n);
		}
		
		std::string getSaverExtension(int n);

		int findSaver(std::string name);


	protected:
		virtual const char *popName(int n)
		{
			return saverName(n);
		}

		virtual size_t popCount(void) 
		{
			return saverCount();
		}
	};

		//! Wrapper for LWImageList
	//! @ingroup Globals
	class Image : public PopUpCallback, public Storeable, protected GlobalBase<LWImageList>
	{
	private:
		LWImageID id = nullptr;
		struct
		{
			bool keepAspect : 1;
			bool checkered : 1;
		} mFlags{ true, true };
	public:
		virtual const char *popName(int n);

		virtual size_t popCount(void); 

		Image(LWImageID _id = nullptr) : id(_id)
		{
			updateGlobal();
			globPtr->saverNotifyAttach(this, ImageSaverCB);
		}

		Image(const Image &from)
		{
			id = from.id;
			mFlags = from.mFlags;
			updateGlobal();
			globPtr->saverNotifyAttach(this, ImageSaverCB);
		}

		virtual ~Image()
		{
			globPtr->saverNotifyDetach(this);
		}

		LWImageID saverNotify() const { return id; }

		static LWImageID ImageSaverCB(LWInstance inst)
		{
			if (inst == nullptr) return nullptr;
			Image *host = static_cast<Image *>(inst);
			return host->saverNotify();
		}

		Image &operator= (const Image &from)
		{
			if (this != &from)
			{
				id = from.id;
				mFlags = from.mFlags;
			}
			return *this;
		}
		//! Use the image aspect ratio when drawing a xpanel preview. Fit to view otherwise 
		void PreviewKeepAspect(const bool keep = true) { mFlags.keepAspect = keep; }
		void CheckeredBackground(const bool on = true) { mFlags.checkered = on; }
		void SetID(LWImageID _id) {id = _id;}
		LWImageID getID() const {return id;}
		//! Get the value returned from a pop-up 
		void GetPopUp(int i);
		//! Set the value for a pop-up
		int SetPopUp();

		bool isValid() {return id != 0;}
		void first(void) {id = globPtr->first();}
		void next(void) {id = globPtr->next(id);}
		void clear(void) {globPtr->clear(id);}
		void load(const char *filename) {id = globPtr->load(filename);}
		void load(const std::string filename) { load(filename.c_str()); }
		const char *name() const {
			static const char none[] = "(none)";
			if (id) return globPtr->name(id);
			return none;
		}
		const char *filename(LWFrame frame = 0) const {return globPtr->filename(id, frame);}
		bool replace (const std::string &newFile)
		{
			return globPtr->replace(id, newFile.c_str()) != 0;
		}

		bool reload()
		{
			return globPtr->replace(id, filename()) != 0;
		}

		LWImageID getNumImage(int n);

		int getImageNum(LWImageID i);

		bool isColor (void) const
		{
			return (globPtr->isColor(id) != 0);
		}
		bool hasAlpha (void) const 
		{
			return (globPtr->hasAlpha(id) != 0);
		}		
		void size (int &w, int &h) const 
		{
			globPtr->size(id, &w, &h);
		}
		void GetSize(int &w, int &h) const
		{
			globPtr->size(id, &w, &h);
		}
		int getWidth (void) const 
		{
			int w, h;
			globPtr->size(id, &w, &h);
			return w;
		}
		int getHeight (void) const 
		{
			int w, h;
			globPtr->size(id, &w, &h);
			return h;
		}

		float getAspect() const
		{
			int w, h;
			globPtr->size(id, &w, &h);
			return static_cast<double>(w) / static_cast<double>(h);
		}
		
#ifdef WIN32
#ifdef RGB
#undef RGB
#endif
#endif

		void RGB (int x, int y, LWBufferValue val[3]) const 
			{
			globPtr->RGB(id, x, y, val);
		}
		void needAA ()
		{
			if (id) globPtr->needAA(id);
		}

		void GetRGBSpot (double x, double y, double spotSize, bool blend, double rgb[3])
		{
			if (id) globPtr->RGBSpot(id, x, y, spotSize, blend ? 1 : 0, rgb);
		}

		double GetAlphaSpot (double x, double y, double spotSize, bool blend)
		{
			if (id) return globPtr->alphaSpot(id, x, y, spotSize, blend ? 1 : 0);
			return 0.0;
		}

		double GetLumaSpot (double x, double y, double spotSize, bool blend)
		{
			if (id) return globPtr->lumaSpot(id, x, y, spotSize, blend ? 1 : 0);
			return 0.0;
		}

		LWBufferValue alpha (int x, int y) const 
		{
			return globPtr->alpha(id, x, y);
		}
		
		LWBufferValue luma (int x, int y) const 
		{
			return globPtr->luma(id, x, y);
		}

		double evaluate(double u, double v,
										double dudx, double dvdx, double dudy, double dvdy,
										int pixelBlending, int useMip, double mipStrength,
										LWTextureWrap uWrap, LWTextureWrap vWrap, LWDVector colour) const
		{
			if (id) return globPtr->evaluateImage(id, u, v, dudx, dvdx, dudy, dvdy,
																		pixelBlending, useMip, mipStrength,
																		uWrap, vWrap, colour);
			return 0.0;
		}

		double evaluate(LWNodalProjection proj,
										int pixelBlending, int useMip, double mipStrength,
										LWTextureWrap uWrap, LWTextureWrap vWrap, LWDVector colour) const
		{
			if (id) return globPtr->evaluateImage(id, proj.u, proj.v, proj.dudx, proj.dvdx, proj.dudy, proj.dvdy,
																						pixelBlending, useMip, mipStrength,
																						uWrap, vWrap, colour);
			return 0.0;
		}

		Vector3d evaluateBumpGradient(LWNodalProjection proj,
																	int pixelBlending, int useMip, double mipStrength, double bumpStrength,
																	LWTextureWrap uWrap, LWTextureWrap vWrap) const;

		LWPixmapID evaluate (LWTime t)
		{
			return globPtr->evaluate(id, t);
		}

		virtual LWError Load(const LoadState &ls )
		{
			LWError err = 0;
			id = globPtr->sceneLoad(ls.getState());
			if (id == 0) err = "Could not load image";
			return err;
		}
		virtual LWError Save(const SaveState &ss )
		{
			LWError err = 0;
			globPtr->sceneSave(ss.getState(), id);
			//globPtr->saverNotifyMarkUsage(id);
			return err;
		}
		//! 
		static void DrawImage(LWXPanelID pan, unsigned int cid, LWXPDrAreaID reg, int w, int h);
		//! Draws the image into an XPanel control
		void drawXpanel(LWXPDrAreaID reg, int w, int h);

	};

	lwpp::Vector3d ComputeBump(LWNodalProjection &proj, double dtu, double dtv, double displace, double bumpStrength);
	
	#define IMAGECHANGE_COMRING "lwppImageChange"
	
	// register a comRing for image changes
	/*
	 * The Comring is called "lwppImageChange".
	 * The eventCode is an image change eventCode.
	 * The data payload is the imageID.
	 */
	void RegisterImageChangeComRing();
	// unregister the Comring
	void UnregisterImageChangeComRing();
}
#endif // LWPP_IMAGE_H
