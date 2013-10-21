/// Author:  Matthew Johnson
/// Version: 1.0
/// Date:    March 13, 2006
/// Notice:  You are free to use this code as you wish.  There are no guarantees whatsoever about
/// its usability or fitness of purpose.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Nexus.Windows.Forms;
using System.Drawing.Imaging;
using System.Drawing.Printing;

namespace PieChartTest
{
  public partial class Form1 : Form
  {
    public Form1()
    {
      InitializeComponent();

      PieChart1.Items.Add(new PieChartItem(10, Color.BurlyWood, "Tan", "Tan tool tip", 0));
      PieChart1.Items.Add(new PieChartItem(10, Color.Gold, "Gold", "Gold tool tip", 0));
      PieChart1.Items.Add(new PieChartItem(10, Color.Chocolate, "Brown", "Brown tool tip", 50));
      PieChart1.Items.Add(new PieChartItem(20, Color.DarkRed, "Red", "Red tool tip", 0));


      PieChart1.ItemStyle.SurfaceAlphaTransparency = 0.75F;
      PieChart1.FocusedItemStyle.SurfaceAlphaTransparency = 0.75F;
      PieChart1.FocusedItemStyle.SurfaceBrightnessFactor = 0.3F;

      cmbTextMode.Items.Add(PieChart.TextDisplayTypes.Always);
      cmbTextMode.Items.Add(PieChart.TextDisplayTypes.FitOnly);
      cmbTextMode.Items.Add(PieChart.TextDisplayTypes.Never);
      cmbTextMode.SelectedIndex = 0;

      trkRotation.Value = (int)(PieChart1.Rotation * 180 / Math.PI);
      trkIncline.Value = (int)(PieChart1.Inclination * 180 / Math.PI);
      trkThickness.Value = (int)(PieChart1.Thickness);
      trkRadius.Value = (int)(PieChart1.Radius);

      trkEdgeBrightness.Value = (int)(PieChart1.ItemStyle.EdgeBrightnessFactor * 100);
      trkFocusedEdgeBrightness.Value = (int)(PieChart1.FocusedItemStyle.EdgeBrightnessFactor * 100);
      trkSurfaceBrightness.Value = (int)(PieChart1.ItemStyle.SurfaceBrightnessFactor * 100);
      trkFocusedSurfaceBrightness.Value = (int)(PieChart1.FocusedItemStyle.SurfaceBrightnessFactor * 100);
      trkSurfaceTransparency.Value = (int)(PieChart1.ItemStyle.SurfaceAlphaTransparency * 100);
      trkFocusedSurfaceTransparency.Value = (int)(PieChart1.FocusedItemStyle.SurfaceAlphaTransparency * 100);

      chkAutoSizeRadius.Checked = PieChart1.AutoSizePie;
      chkShowEdges.Checked = PieChart1.ShowEdges;
      chkShowToolTips.Checked = PieChart1.ShowToolTips;

      cmbTextMode.SelectedItem = PieChart1.TextDisplayMode;

      propertyGrid1.SelectedObject = new ItemCollectionProxy(PieChart1);
    }

    private void trkRotation_Scroll(object sender, EventArgs e)
    {
      PieChart1.Rotation = (float)(trkRotation.Value * Math.PI / 180);
    }

    private void trkIncline_Scroll(object sender, EventArgs e)
    {
      PieChart1.Inclination = (float)(trkIncline.Value * Math.PI / 180);
    }

    private void trkThickness_Scroll(object sender, EventArgs e)
    {
      PieChart1.Thickness = trkThickness.Value;
    }

    private void trkRadius_Scroll(object sender, EventArgs e)
    {
      PieChart1.Radius = trkRadius.Value;
    }

    private void PieChart1_AutoSizePieChanged(object sender, EventArgs e)
    {
      trkRadius.Enabled = !PieChart1.AutoSizePie;
    }

    private void chkAutoSizeRadius_CheckedChanged(object sender, EventArgs e)
    {
      PieChart1.AutoSizePie = chkAutoSizeRadius.Checked;
    }

    private void chkShowEdges_CheckedChanged(object sender, EventArgs e)
    {
      PieChart1.ShowEdges = chkShowEdges.Checked;
    }

    private void chkShowToolTips_CheckedChanged(object sender, EventArgs e)
    {
      PieChart1.ShowToolTips = chkShowToolTips.Checked;
    }

    private void trkSurfaceTransparency_Scroll(object sender, EventArgs e)
    {
      PieChart1.ItemStyle.SurfaceAlphaTransparency = (float)trkSurfaceTransparency.Value / 100;
    }

    private void trkFocusedSurfaceTransparency_Scroll(object sender, EventArgs e)
    {
      PieChart1.FocusedItemStyle.SurfaceAlphaTransparency = (float)trkFocusedSurfaceTransparency.Value / 100;
    }

    private void trkEdgeBrightness_Scroll(object sender, EventArgs e)
    {
      PieChart1.ItemStyle.EdgeBrightnessFactor = (float)trkEdgeBrightness.Value / 100;
    }

    private void trkFocusedEdgeBrightness_Scroll(object sender, EventArgs e)
    {
      PieChart1.FocusedItemStyle.EdgeBrightnessFactor = (float)trkFocusedEdgeBrightness.Value / 100;
    }

    private void trkSurfaceBrightness_Scroll(object sender, EventArgs e)
    {
      PieChart1.ItemStyle.SurfaceBrightnessFactor = (float)trkSurfaceBrightness.Value / 100;
    }

    private void trkFocusedSurfaceBrightness_Scroll(object sender, EventArgs e)
    {
      PieChart1.FocusedItemStyle.SurfaceBrightnessFactor = (float)trkFocusedSurfaceBrightness.Value / 100;
    }

    private void cmbTextMode_SelectedIndexChanged(object sender, EventArgs e)
    {
      if (cmbTextMode.SelectedItem != null)
        PieChart1.TextDisplayMode = (PieChart.TextDisplayTypes)cmbTextMode.SelectedItem;
    }

    private void OnItemFocusChanging(object sender, PieChart.PieChartItemFocusEventArgs e)
    {
      if (e.NewItem != null && e.NewItem.Text != null)
        this.Text = "PieChartTest - " + e.NewItem.Text;
      else
        this.Text = "PieChartTest - [no item focused]";
    }

    private void OnItemClicked(object sender, PieChart.PieChartItemEventArgs e)
    {
      if (e.Item.Tag == null || (bool)e.Item.Tag == true)
      {
        e.Item.Offset += 50;
        e.Item.Tag = false;
      }
      else
      {
        e.Item.Offset = Math.Max(0, e.Item.Offset - 50);
        e.Item.Tag = true;
      }
    }

    private void mnuFileSaveAs_Click(object sender, EventArgs e)
    {
      SaveFileDialog dlg = new SaveFileDialog();
      dlg.Filter = "PNG Image|*.png|JPEG Image|*.jpg;*.jpeg|Bitmap Image|*.bmp|GIF Image|*.gif";

      Size imageSize;
      if (PieChart1.AutoSizePie)
        imageSize = new Size(PieChart1.Bounds.Width, PieChart1.Bounds.Height);
      else
        imageSize = PieChart1.GetChartSize(PieChart1.Padding);

      if (dlg.ShowDialog() == DialogResult.OK)
      {
        switch (dlg.FilterIndex)
        {
          case 1:
            PieChart1.SaveAs(dlg.FileName, ImageFormat.Png, imageSize, PieChart1.Padding);
            break;
          case 2:
            PieChart1.SaveAs(dlg.FileName, ImageFormat.Jpeg, imageSize, PieChart1.Padding);
            break;
          case 3:
            PieChart1.SaveAs(dlg.FileName, ImageFormat.Bmp, imageSize, PieChart1.Padding);
            break;
          case 4:
            PieChart1.SaveAs(dlg.FileName, ImageFormat.Gif, imageSize, PieChart1.Padding);
            break;
          default:
            throw new Exception("Unknown file filter.");
        }
      }
    }

    private void mnuFilePrint_Click(object sender, EventArgs e)
    {
      PrintDialog dlg = new PrintDialog();
      dlg.Document = new PrintDocument();
      PieChart1.AttachPrintDocument(dlg.Document);

      if (dlg.ShowDialog() == DialogResult.OK)
      {
        dlg.Document.Print();
      }
    }

    private void mnuFileExit_Click(object sender, EventArgs e)
    {
      this.Close();
    }
  }
}