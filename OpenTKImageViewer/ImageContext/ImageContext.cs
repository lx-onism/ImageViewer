﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using OpenTK.Graphics.OpenGL4;
using OpenTKImageViewer.glhelper;
using OpenTKImageViewer.Tonemapping;
using OpenTKImageViewer.Utility;

namespace OpenTKImageViewer.ImageContext
{
    public delegate void ChangedLayerHandler(object sender, EventArgs e);

    public delegate void ChangedMipmapHanlder(object sender, EventArgs e);

    public delegate void ChangedImagesHandler(object sender, EventArgs e);

    public delegate void ChangedFilteringHandler(object sender, EventArgs e);

    public delegate void ChangedGrayscaleHandler(object sender, EventArgs e);

    public class ImageContext
    {
        #region Structures and Enums

        private class ImageData
        {
            public ImageLoader.Image image;
            public TextureArray2D TextureArray2D;

            public ImageData(ImageLoader.Image image)
            {
                this.image = image;
            }
        }

        public enum GrayscaleMode
        {
            Disabled,
            Red,
            Green,
            Blue,
            Alpha
        }

        #endregion

        #region Private Member

        private readonly List<ImageData> images = new List<ImageData>();
        private uint activeMipmap = 0;
        private uint activeLayer = 0;
        private bool linearInterpolation = false;
        private GrayscaleMode grayscale = GrayscaleMode.Disabled;
        private bool displayColorBeforeTonemapping = true;
        private readonly ImageConfiguration[] finalTextures = new ImageConfiguration[2];

        #endregion

        #region Public Properties

        public bool LinearInterpolation
        {
            get => linearInterpolation;
            set
            {
                if (value == linearInterpolation) return;
                linearInterpolation = value;
                OnChangedFiltering();
            }
        }

        public GrayscaleMode Grayscale
        {
            get => grayscale;
            set
            {
                if (value == grayscale) return;
                grayscale = value;
                OnChangedGrayscale();
            }
        }

        public uint ActiveMipmap
        {
            get { return activeMipmap; }
            set
            {
                if (value != activeMipmap && value < GetNumMipmaps())
                {
                    activeMipmap = value;
                    OnChangedMipmap();
                }
            }
        }

        public uint ActiveLayer
        {
            get { return activeLayer; }
            set
            {
                if (value != activeLayer && value < GetNumLayers())
                {
                    activeLayer = value;
                    OnChangedLayer();
                }
            }
        }

        public TonemapperManager Tonemapper { get; } = new TonemapperManager();

        // this will determine if the cpu cached textures will be acquired directly after combining the images or after tonemapping
        public bool DisplayColorBeforeTonemapping {
            get => displayColorBeforeTonemapping;
            set
            {
                if (value != displayColorBeforeTonemapping)
                {
                    displayColorBeforeTonemapping = value;
                    if (value)
                        foreach (var imageConfiguration in finalTextures)
                        {
                            imageConfiguration.RecomputeImage = true;
                        }
                    else
                        // since the images after the tonemapping is requested nothing needs to be recomputed
                        foreach (var imageConfiguration in finalTextures)
                        {
                            imageConfiguration.RecomputeCpuTexture = true;
                        }
                }
            }
        }

        public TextureCache TextureCache { get; }
        #endregion

        #region Public Getter

        public ImageConfiguration GetImageConfiguration(int id)
        {
            return finalTextures[id];
        }

        public TextureArray2D GetImageTexture(int id)
        {
            return images[id].TextureArray2D;
        }

        public int GetNumImages()
        {
            return images.Count;
        }

        public int GetNumMipmaps()
        {
            if (images.Count == 0)
                return 0;
            return images[0].image.GetNumMipmaps();
        }

        public int GetNumLayers()
        {
            if (images.Count == 0)
                return 0;
            return images[0].image.Layers.Count;
        }

        public int GetWidth(int mipmap)
        {
            Debug.Assert(images.Count != 0);
            return images[0].image.GetWidth(mipmap);
        }

        public int GetHeight(int mipmap)
        {
            Debug.Assert(images.Count != 0);
            return images[0].image.GetHeight(mipmap);
        }

        public string GetFilename(int image)
        {
            Debug.Assert((uint)(image) < images.Count);
            return images[image].image.Filename;
        }

        /// <summary>
        /// checks if all images are grayscale images
        /// </summary>
        /// <returns>true if all images are grayscale images</returns>
        public bool HasOnlyGrayscale()
        {
            return images.All(imageData => imageData.image.IsGrayscale());
        }

        /// <summary>
        /// checks if any image has alpha channel
        /// </summary>
        /// <returns>true if any image has alpha channel</returns>
        public bool HasAlpha()
        {
            return images.Any(imageData => imageData.image.HasAlpha());
        }

        /// <summary>
        /// checks if any image is hdr
        /// </summary>
        /// <returns>true if any image is hdr</returns>
        public bool HasHdr()
        {
            return images.Any(imageData => imageData.image.IsHdr());
        }

        // TODO update
        public byte[] GetCurrentImageData(int level, int layer, PixelFormat format, PixelType type, out int width,
            out int height)
        {
            width = GetWidth(level);
            height = GetHeight(level);
            if (finalTextures[0] == null)
                return null;

            return finalTextures[0].Texture.GetData(level, layer, format, type, out width, out height);
        }

        public CpuTexture GetCpuTexture()
        {
            // TODO determine active texture
            return finalTextures[0].CpuCachedTexture;
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// tries to add the image. Throws Exception if image could not be added
        /// </summary>
        /// <param name="image"></param>
        public void AddImage(ImageLoader.Image image)
        {
            if(IsImageProcessing())
                throw new Exception("Images cannot be added while an operation is running");

            // only add if layout is consistent
            if (images.Count > 0)
            {
                var i = images[0].image;
                if (image.Layers.Count != i.Layers.Count)
                    throw new Exception($"Inconsistent amount of layers. Expected {i.Layers.Count} got {image.Layers.Count}");

                if (image.GetNumMipmaps() != i.GetNumMipmaps())
                    throw new Exception($"Inconsistent amount of mipmaps. Expected {i.GetNumMipmaps()} got {image.GetNumMipmaps()}");

                // test mipmaps
                for (int level = 0; level < image.GetNumMipmaps(); ++level)
                {
                    if (image.GetWidth(level) != i.GetWidth(level) || image.GetHeight(level) != i.GetHeight(level))
                        throw new Exception($"Inconsistent mipmaps dimension. Expected {i.GetWidth(level)}x{i.GetHeight(level)}" +
                                            $" got {image.GetWidth(level)}x{image.GetHeight(level)}");
                }
            }

            images.Add(new ImageData(image));
            OnChangedImages();
            if(HasOnlyGrayscale())
                Grayscale = GrayscaleMode.Red;
        }

        // TODO update
        public void BindFinalTextureAs2DSamplerArray(int slot)
        {
            finalTextures[0].Texture?.Bind(slot, LinearInterpolation);
        }

        public void BindFinalTextureAsCubeMap(int slot)
        {
            finalTextures[0].Texture ?.BindAsCubemap(slot, LinearInterpolation);
        }

        /// <summary>
        /// should be called before drawing the final image in order to update its contents if required
        /// </summary>
        /// <return>true if image is ready to be drawn, false if image has to be processed</return>
        public bool Update()
        {
            if (images.Count == 0)
                return true;

            // create gpu textures for newly added images
            foreach (var imageData in images)
            {
                if (imageData.TextureArray2D == null)
                    imageData.TextureArray2D = new TextureArray2D(imageData.image);
            }

            foreach (var imageConfiguration in finalTextures)
            {
                if (!imageConfiguration.Update())
                    return false; // something has to be updated and cannot be drawn
            }

            return true;
        }

        private IStepable FindStepable()
        {
            foreach (var imageConfiguration in finalTextures)
            {
                if (imageConfiguration.TonemappingStepable != null)
                    return imageConfiguration.TonemappingStepable;
            }
            return null;
        }

        public float GetImageProcess()
        {
            // find first stepable
            var tonemappingStepable = FindStepable();
            if (tonemappingStepable == null)
                return 1.0f;
            return (float)tonemappingStepable.CurrentStep();
        }

        public string GetImageLoadingDescription()
        {
            var tonemappingStepable = FindStepable();
            if (tonemappingStepable == null)
                return "";
            return tonemappingStepable.GetDescription();
        }

        public bool IsImageProcessing()
        {
            var tonemappingStepable = FindStepable();
            return tonemappingStepable != null;
        }

        public void AbortImageProcessing()
        {
            if (!IsImageProcessing()) return;

            // TODO restore old image?
            foreach (var imageConfiguration in finalTextures)
            {
                imageConfiguration.AbortImageCalculation();
            }
        }

        #endregion

        #region Events

        public event ChangedLayerHandler ChangedLayer;
        public event ChangedMipmapHanlder ChangedMipmap;
        public event ChangedImagesHandler ChangedImages;
        public event ChangedFilteringHandler ChangedFiltering;
        public event ChangedGrayscaleHandler ChangedGrayscale;

        protected virtual void OnChangedLayer()
        {
            ChangedLayer?.Invoke(this, EventArgs.Empty);
        }

        protected virtual void OnChangedMipmap()
        {
            ChangedMipmap?.Invoke(this, EventArgs.Empty);
        }

        protected virtual void OnChangedImages()
        {
            ChangedImages?.Invoke(this, EventArgs.Empty);
        }

        protected virtual void OnChangedFiltering()
        {
            ChangedFiltering?.Invoke(this, EventArgs.Empty);
        }

        protected virtual void OnChangedGrayscale()
        {
            ChangedGrayscale?.Invoke(this, EventArgs.Empty);
        }

        #endregion


        public ImageContext(List<ImageLoader.Image> images)
        {
            TextureCache = new TextureCache(this);

            for (var i = 0; i < finalTextures.Length; ++i)
            {
                finalTextures[i] = new ImageConfiguration(this)
                // only first is active by default
                { Active = i == 0};
            }
                

            // on changed events
            Tonemapper.ChangedSettings += (sender, args) =>
            {
                foreach (var toneConfiguration in finalTextures)
                {
                    // only recompute if tonemappers would be used
                    if (toneConfiguration.UseTonemapper)
                        toneConfiguration.RecomputeImage = true;
                }
            };
            if (images != null)
            {
                foreach (var image in images)
                {
                    AddImage(image);
                }
            }
        }
    }
}
