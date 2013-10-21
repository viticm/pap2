using System;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Design;
using System.Windows.Forms.ComponentModel;
using System.Windows.Forms.Design;
using System.Windows.Forms;

/// <summary>
///	Mikael Wiberg, Copyright 2003, 2004
///		My mikwib address can be found on hotmail.com (usual HoTMaiL spam filters)
///		My mick address can be found on ar.com.au (heavy spam filters on, harldy anything gets through, START the subject with C# and it will probably go through)
///		My mik address can be found on netatonce.net (heavy spam filters on, harldy anything gets through, START the subject with C# and it will probably go through)
///	
///	Feel free to use the MWTreeView as you wish, as long as you do not take credit for it yourself (see next line).
///	However if it is used in commercial projects or applications it is NOT, I repeat NOT, free. Please contact me for further information.
///		Note that this differs from previous versions of the MWTreeView and I have made this decision in order to be duly compensated for my work
///			and time spent.
///	Use the MWTreeView at your own risk. I take no responsibility for ill effects happening directly or indirectly through the use of the MWTreeView.
/// </summary>
namespace MWControlSuite
{
	/// <summary>
	/// EditorHatchStyle is used in conjunction with the EditorHatchStyleUI class.
	/// </summary>
	public class EditorHatchStyle : System.Drawing.Design.UITypeEditor
	{
		#region Variables

		private PropertyDescriptor pd = null;
		private object oInstance = null;

		private IWindowsFormsEditorService iwfes = null;

		#endregion Variables



		#region Overridden Methods and EventHandlers for UITypeEditor

		#region Methods for setting the value

		/// <summary>
		/// This enables the button for the DropDown to appear in the properties window.
		/// </summary>
		/// <param name="itdc">Standard ITypeDescriptorContext object.</param>
		/// <returns>The desired UITypeEditorEditStyle (in a DropDown).</returns>
		public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext itdc) 
		{
			if(itdc != null && itdc.Instance != null) 
			{
				return UITypeEditorEditStyle.DropDown;
			}

			return base.GetEditStyle(itdc);
		}

		/// <summary>
		/// This takes care of the actual value-change of the property.
		/// </summary>
		/// <param name="itdc">Standard ITypeDescriptorContext object.</param>
		/// <param name="isp">Standard IServiceProvider object.</param>
		/// <param name="value">The value as an object.</param>
		/// <returns>The new value as an object.</returns>
		public override object EditValue(ITypeDescriptorContext itdc, IServiceProvider isp, object value)
		{
			if(itdc != null && itdc.Instance != null && isp != null) 
			{
				iwfes = (IWindowsFormsEditorService)isp.GetService(typeof(IWindowsFormsEditorService));

				if(iwfes != null)
				{
					HatchStyle hs = HatchStyle.Weave;

					//if(itdc.PropertyDescriptor.GetValue(itdc.Instance) is HatchStyle)
					if(value is HatchStyle)
					{
						hs = (HatchStyle)itdc.PropertyDescriptor.GetValue(itdc.Instance);
						pd = itdc.PropertyDescriptor;
						oInstance = itdc.Instance;
					}

					//EditorHatchStyleUI ehsui = new EditorHatchStyleUI(hs, Color.White, Color.Black); //FIX THIS XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
					EditorHatchStyleUI ehsui = new EditorHatchStyleUI(HatchStyle.Trellis, Color.White, Color.Black);
					ehsui.IWFES = iwfes;
					ehsui.ITDC = itdc;
					ehsui.HatchStyle = (HatchStyle)value;
					ehsui.HatchStyleChanged += new EditorHatchStyleUI.MWHatchStyleEventHandler(ValueChanged);

					iwfes.DropDownControl(ehsui);
					value = ehsui.HatchStyle;
				}
			}

			return value;
		}

		#endregion Methods for setting the value



		#region Methods for painting the value

		/// <summary>
		/// True if Custom Painting or false otherwise.
		/// </summary>
		/// <param name="itdc">Standard ITypeDescriptorContext object.</param>
		/// <returns>True if Custom Painting or false otherwise.</returns>
		public override bool GetPaintValueSupported(ITypeDescriptorContext itdc)
		{
			return true;
		}

		/// <summary>
		/// Paint the value in Visual Studio's (or wherever it is used) Property Window.
		/// </summary>
		/// <param name="e">Standard PaintValueEventArgs object.</param>
		public override void PaintValue(PaintValueEventArgs e)
		{
			//See e.Graphics.Restore further down.
			GraphicsState gs = e.Graphics.Save();

			StringFormat strfmt = StringFormat.GenericDefault;
			strfmt.Alignment = StringAlignment.Center;
			strfmt.LineAlignment = StringAlignment.Center;

			e.Graphics.FillRectangle(new HatchBrush((HatchStyle)e.Value, Color.Black, Color.White), e.Bounds);

			//Important. Without this Visual Studio (or whatever) cannot draw its Property Window properly.
			e.Graphics.Restore(gs);

			base.PaintValue(e);
		}

		#endregion Methods for painting the value

		#endregion Overridden Methods and EventHandlers for UITypeEditor



		#region Help Methods for setting the value

		/// <summary>
		/// Standard ValueChanged EventHandler for the EditorHatchStyleUI ehsui.
		/// </summary>
		/// <param name="sender">Standard sender object.</param>
		/// <param name="e">Standard MWHatchStyleEventArgs object.</param>
		private void ValueChanged(object sender, MWControlSuite.MWHatchStyleEventArgs e)
		{
			if(pd != null && oInstance != null)
			{
				//This does not work, i.e. it does not update the property in Visual Studio.
				//Create a PBPercentEventArgs etc and it will work; look at EditorShadowDirection (names have to match I think
				//	so name the property PBPercent in the PBPercentEventArgs class).
				pd.SetValue(oInstance, e.NewHatchStyle);
			}
		}

		#endregion Help Methods for setting the value

	}
}
