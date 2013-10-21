//    Copyright (C) 2005  Riccardo Marzi
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
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Fireball.Windows.Forms
{
    public partial class ImagePreview : UserControl
    {

        public ImagePreview()
        {
            InitializeComponent();
            tbZoomMinus.Enabled = false;
            tbZoomPlus.Enabled = false;
            tbZoom100.Enabled = false;
        }

        private void tbZoomFit_Click(object sender, EventArgs e)
        {
            picturePreview.ZoomFit();
        }

        private void tbZoom100_Click(object sender, EventArgs e)
        {
            picturePreview.ZoomOriginal();
            UpdateButtons();
        }

        private void tbZoomMinus_Click(object sender, EventArgs e)
        {
            picturePreview.DecreaseZoom();
            UpdateButtons();
        }

        private void tbZoomPlus_Click(object sender, EventArgs e)
        {
            picturePreview.IncreaseZoom();
            UpdateButtons();
        }

        public void LoadPreview(Bitmap image)
        {
            picturePreview.Info = image.Size.ToString();
            picturePreview.Image = image;
            tbZoomMinus.Enabled = true;
            tbZoomPlus.Enabled = true;
            tbZoom100.Enabled = true;
        }
        public void LoadPreview(string filename)
        {
            picturePreview.Info = System.IO.Path.GetFileNameWithoutExtension(filename);
            picturePreview.Load(filename);
            tbZoomMinus.Enabled = true;
            tbZoomPlus.Enabled = true;
            tbZoom100.Enabled = true;
        }

        private void UpdateButtons()
        {
            tbZoomMinus.Enabled = picturePreview.CanDecreaseZoom();
            tbZoomPlus.Enabled = picturePreview.CanIncreaseZoom();
        }
    }
}
