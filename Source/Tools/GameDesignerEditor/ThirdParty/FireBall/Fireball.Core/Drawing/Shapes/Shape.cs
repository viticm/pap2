
//    Copyright (C) 2005  Riccardo Marzi  <riccardo@dotnetfireball.net>
//
//    Copyright (C) 2005  Sebastian Faltoni <sebastian@dotnetfireball.net>
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Lesser General Public
//    License as published by the Free Software Foundation; either
//    version 2.1 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace Fireball.Drawing.Shapes
{
    /// <summary>
    /// Base class for all shapes
    /// </summary>
	public abstract class Shape  
	{

        private Pen _pen;
        private Brush _brush;
		private float _rotation;

        #region Protected

        protected GraphicsPath InternalPath;
        protected PointF InternalRotationBasePoint;

        #endregion

        #region Abstract

        public abstract RectangleF Bounds { get;}
		public abstract PointF Center { get;set;}
		public abstract PointF Location { get;set;}

        public abstract Image GetThumb(ShapeThumbSize thumbSize);
		public abstract void Paint(Graphics graphics, SmoothingMode smoothingMode);
		public abstract void Rotate(float value, PointF basePoint);
		public abstract void Translate(float dx, float dy);

        protected abstract void UpdatePath();

        #endregion

        #region Virtual

        /// <summary>
        /// Return the GraphicsPath used for this shape
        /// </summary>
        public virtual GraphicsPath Path 
		{
			get
			{
				return InternalPath;
			}
		}

        /// <summary>
        /// The Shape size
        /// </summary>
		public virtual SizeF Size
		{
			get
			{
				return Bounds.Size;
			}
		}

        /// <summary>
        /// Pen used for this Shape
        /// </summary>
		public virtual Pen Pen
		{
			get
			{
				return _pen;
			}
			set
			{
				_pen = value;
			}
		}
        /// <summary>
        /// Brush used for filling this shape
        /// </summary>
		public virtual Brush Brush
		{
			get
			{
				return _brush;
			}
			set
			{
				_brush = value;
			}
		}

        /// <summary>
        /// Rotation applied to this shape
        /// </summary>
		public virtual float Rotation 
		{
			get
			{
				return _rotation;
			}
			set
			{
				_rotation = value;
				UpdatePath();
			}
		}

        /// <summary>
        /// Draw the shape on the Graphics
        /// </summary>
        /// <param name="graphics"></param>
		public virtual void Paint(Graphics graphics)
		{
			this.Paint(graphics, graphics.SmoothingMode);
		}

        /// <summary>
        /// Update the Shape
        /// </summary>
		public virtual void Update()
		{
			this.UpdatePath();
		}

        /// <summary>
        /// Rotate the shape
        /// </summary>
        /// <param name="value">Rotation degree</param>
		public virtual void Rotate(float value)
		{
			this.Rotate(value, this.Center);
        }

        #endregion

        #region Overrides

        public override string ToString()
        {
            return "{Fireball.Drawing.Shapes.Shape}";
        }

        #endregion

    }
}