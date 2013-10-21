
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

	public class ShapePie  :ShapeArc,IFillable
	{

		internal ShapePie()
		{

		}
        public ShapePie(float left, float top, float width, float height, float startAngle, float sweepAngle, float rotation, Pen pen)
            : base(left, top, width, height, startAngle, sweepAngle, rotation, pen) { }

        protected override void UpdatePath()
        {
            InternalPath = new GraphicsPath();
            InternalPath.AddPie(this.Left, this.Top, this.Width, this.Height, this.StartAngle, this.SweepAngle);

            Matrix mtx = new Matrix();
            mtx.RotateAt(this.Rotation, InternalRotationBasePoint);
            InternalPath.Transform(mtx);
        }

        #region IFillable Members


        public void Fill(Graphics graphics)
        {
            graphics.FillPath(this.Brush, InternalPath);
        }

        #endregion
    }
}