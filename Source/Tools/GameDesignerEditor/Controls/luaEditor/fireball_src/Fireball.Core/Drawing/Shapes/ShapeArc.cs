
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
    /// Arc shape
    /// </summary>
	public class ShapeArc  :ShapeLine,IRectangle
    {

        private float _left, _top, _width, _height, _startAngle, _sweepAngle;

        #region Constructors

        internal ShapeArc()
        {
        }

        public ShapeArc(float left, float top, float width, float height, float startAngle, float sweepAngle, float rotation, Pen pen)
        {
            _left = left;
            _top = top;
            _width = width;
            _height = height;
            _startAngle = startAngle;
            _sweepAngle = sweepAngle;

            this.Rotation = rotation;
            this.Pen = pen;

            UpdatePath();
        }

        #endregion

        #region Overrides

        protected override void UpdatePath()
        {
            InternalPath = new GraphicsPath();
            InternalPath.AddArc(_left, _top, _width, _height, _startAngle, _sweepAngle);

            Matrix mtx = new Matrix();
            mtx.RotateAt(this.Rotation, InternalRotationBasePoint);
            InternalPath.Transform(mtx);
        }

        public override PointF StartPoint
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }
        public override PointF EndPoint
        {
            get
            {
                throw new NotImplementedException();
            }
            set
            {
                throw new NotImplementedException();
            }
        }

        public override PointF Location
        {
            get
            {
                return new PointF(_left,_top);
            }
            set
            {
                _left = value.X;
                _top = value.Y;
            }
        }
        public override RectangleF Bounds
        {
            get
            {
                return new RectangleF(_left, _top, _width, _height);
            }
        }

        #endregion

        #region IRectangle Members

        /// <summary>
        /// Left position of this Shape
        /// </summary>
        public float Left
        {
            get
            {
                return _left;
            }
            set
            {
                _left = value;
                UpdatePath();
            }
        }

        /// <summary>
        /// Top position of this Shape
        /// </summary>
        public float Top
        {
            get
            {
                return _top;
            }
            set
            {
                _top = value;
                UpdatePath();
            }
        }

        /// <summary>
        /// Width of this Shape
        /// </summary>
        public float Width
        {
            get
            {
                return _width;
            }
            set
            {
                _width = value;
                UpdatePath();
            }
        }

        /// <summary>
        /// Height of this Shape
        /// </summary>
        public float Height
        {
            get
            {
                return _height;
            }
            set
            {
                _height = value;
                UpdatePath();
            }
        }

        #endregion

        #region Virtual Members

        /// <summary>
        /// Start Angle for the Arc Shape
        /// </summary>
        public virtual float StartAngle
        {
            get
            {
                return _startAngle;
            }
            set
            {
                _startAngle = value;
                UpdatePath();
            }
        }

        /// <summary>
        /// Sweep Angle for the Arc Shape
        /// </summary>
        public virtual float SweepAngle
        {
            get
            {
                return _sweepAngle;
            }
            set
            {
                _sweepAngle = value;
                UpdatePath();
            }
        }

        #endregion
    }
}