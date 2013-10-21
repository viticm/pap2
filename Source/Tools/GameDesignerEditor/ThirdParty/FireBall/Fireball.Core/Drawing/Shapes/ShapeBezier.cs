

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

	public class ShapeBezier  :ShapeLine
	{
		private float _xStart, _yStart, _xFirstControl, _yFirstControl, _xSecondControl, _ySecondControl, _xEnd, _yEnd;

		#region Constructors

		public ShapeBezier(float xStartPoint, float yStartPoint, float xFirstControlPoint, float yFirstControlPoint, float xSecondControlPoint, float ySecondControlPoint, float xEndPoint, float yEndPoint)
			: this(xStartPoint, yStartPoint, xFirstControlPoint, yFirstControlPoint, xSecondControlPoint, ySecondControlPoint, xEndPoint, yEndPoint, 0, null) { }
		public ShapeBezier(float xStartPoint, float yStartPoint, float xFirstControlPoint, float yFirstControlPoint, float xSecondControlPoint, float ySecondControlPoint, float xEndPoint, float yEndPoint, float rotation)
			: this(xStartPoint, yStartPoint, xFirstControlPoint, yFirstControlPoint, xSecondControlPoint, ySecondControlPoint, xEndPoint, yEndPoint, 0, null) { }
		
		public ShapeBezier(PointF startPoint, PointF firstControlPoint, PointF secondControlPoint, PointF endPoint)
			: this(startPoint.X, startPoint.Y, firstControlPoint.X, firstControlPoint.Y, secondControlPoint.X, secondControlPoint.Y, endPoint.X, endPoint.Y, 0, null) { }
		public ShapeBezier(PointF startPoint, PointF firstControlPoint, PointF secondControlPoint, PointF endPoint,float rotation)
			: this(startPoint.X, startPoint.Y, firstControlPoint.X, firstControlPoint.Y, secondControlPoint.X, secondControlPoint.Y, endPoint.X, endPoint.Y, rotation, null) { }

		public ShapeBezier(float xStartPoint, float yStartPoint, float xFirstControlPoint, float yFirstControlPoint, float xSecondControlPoint, float ySecondControlPoint, float xEndPoint, float yEndPoint, Pen pen)
			: this(xStartPoint, yStartPoint, xFirstControlPoint, yFirstControlPoint, xSecondControlPoint, ySecondControlPoint, xEndPoint, yEndPoint, 0,pen) { }
		public ShapeBezier(float xStartPoint, float yStartPoint, float xFirstControlPoint, float yFirstControlPoint, float xSecondControlPoint, float ySecondControlPoint, float xEndPoint, float yEndPoint, float rotation, Pen pen)
		{
			_xStart = xStartPoint;
			_yStart = yStartPoint;
			_xFirstControl = xFirstControlPoint;
			_yFirstControl = yFirstControlPoint;
			_xSecondControl = xSecondControlPoint;
			_ySecondControl = ySecondControlPoint;
			_xEnd = xEndPoint;
			_yEnd = yEndPoint;

			this.Rotation = rotation;
			this.Pen = pen;

			this.UpdatePath();
		}

		public ShapeBezier(PointF startPoint, PointF firstControlPoint, PointF secondControlPoint, PointF endPoint,Pen pen)
			: this(startPoint.X, startPoint.Y, firstControlPoint.X, firstControlPoint.Y, secondControlPoint.X, secondControlPoint.Y, endPoint.X, endPoint.Y, 0,pen) { }
		public ShapeBezier(PointF startPoint, PointF firstControlPoint, PointF secondControlPoint, PointF endPoint, float rotation,Pen pen)
			: this(startPoint.X, startPoint.Y, firstControlPoint.X, firstControlPoint.Y, secondControlPoint.X, secondControlPoint.Y, endPoint.X, endPoint.Y, rotation,pen) { }

		#endregion

		#region Overrides

		public override RectangleF Bounds
		{
			get
			{
				float left = Math.Min(Math.Min(_xStart, _xEnd), Math.Min(_xFirstControl, _xSecondControl));
				float top = Math.Min(Math.Min(_yStart, _yEnd), Math.Min(_yFirstControl, _ySecondControl));
				float right = Math.Max(Math.Max(_xStart, _xEnd), Math.Max(_xFirstControl, _xSecondControl));
				float bottom = Math.Max(Math.Max(_yStart, _yEnd), Math.Max(_yFirstControl, _ySecondControl));

				return new RectangleF(left, top, right - left, bottom - top);
			}
		}
		public override PointF StartPoint
		{
			get
			{
				return new PointF(_xStart,_yStart);
			}
			set
			{
				_xStart = value.X;
				_yStart = value.Y;
			}
		}
		public override PointF EndPoint
		{
			get
			{
				return new PointF(_xEnd,_yEnd);
			}
			set
			{
				_xEnd = value.X;
				_yEnd = value.Y;
			}
		}

		public override void Translate(float dx, float dy)
		{
			_xStart += dx;
			_xFirstControl += dx;
			_xSecondControl += dx;
			_xEnd += dx;
			_yStart += dy;
			_yFirstControl += dy;
			_ySecondControl += dy;
			_yEnd += dy;
			UpdatePath();
		}
        public override Image GetThumb(ShapeThumbSize thumbSize)
        {
            throw new Exception("The method or operation is not implemented.");
        }
        public override string ToString()
        {
            return "{Fireball.Drawing.Shapes.ShapeBezier}";
        }

        protected override void UpdatePath()
		{
			InternalPath = new GraphicsPath();
			InternalPath.AddBezier(_xStart, _yStart, _xFirstControl, _yFirstControl, _xSecondControl, _ySecondControl, _xEnd, _yEnd);

			Matrix mtx = new Matrix();
			mtx.RotateAt(this.Rotation, InternalRotationBasePoint);
			InternalPath.Transform(mtx);
		}

		#endregion

		#region Virtuals

		public virtual PointF FirstControlPoint
		{
			get
			{
				return new PointF(_xFirstControl, _yFirstControl);
			}
			set
			{
				_xFirstControl = value.X;
				_yFirstControl = value.Y;
				UpdatePath();
			}
		}
		public virtual PointF SecondControlPoint
		{
			get
			{
				return new PointF(_xSecondControl, _ySecondControl);
			}
			set
			{
				_xSecondControl = value.X;
				_ySecondControl = value.Y;
				UpdatePath();
			}
		}

		#endregion
	}
}

