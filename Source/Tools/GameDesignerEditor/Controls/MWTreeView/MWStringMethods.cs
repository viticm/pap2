using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

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
	/// Summary description for MWStringMethods.
	/// </summary>
	public class MWStringMethods
	{
		#region GraphicalString Methods

		#region Width related Methods

		/// <summary>
		/// Get the Width of the supplied Control using its Text, Font and Graphics context.
		/// </summary>
		/// <param name="ctl">Control to measure Text of.</param>
		/// <returns>Width of the Text of the Control supplied.</returns>
		public static int GetGraphicalStringWidth(Control ctl)
		{
			return GetGraphicalStringRect(ctl.CreateGraphics(), ctl.Text, ctl.Font).Width;
		}

		/// <summary>
		/// Get the Width of the supplied string using the supplied Font on the supplied Graphics context.
		/// </summary>
		/// <param name="g">Graphics context object to measure string on.</param>
		/// <param name="str">String to measure.</param>
		/// <param name="fnt">Font to use for string.</param>
		/// <returns>Width of the string supplied.</returns>
		public static int GetGraphicalStringWidth(Graphics g, string str, Font fnt)
		{
			return GetGraphicalStringRect(g, str, fnt).Width;
		}

		#endregion Width related Methods



		#region Rectangle related Methods

		/// <summary>
		/// Get the smallest encompassing Rectangle for the Text of the Control supplied using its Text, Font and Graphics context.
		/// </summary>
		/// <param name="ctl">Control to measure Text of.</param>
		/// <returns>Smallest Rectangle encompassing the Text of the supplied Control.</returns>
		public static Rectangle GetGraphicalStringRect(Control ctl)
		{
			return GetGraphicalStringRect(ctl.CreateGraphics(), ctl.Text, ctl.Font);
		}

		/// <summary>
		/// Get the smallest encompassing Rectangle for the supplied string, Font and Graphics context.
		/// </summary>
		/// <param name="g">Graphics context object to measure string on.</param>
		/// <param name="str">String to measure.</param>
		/// <param name="fnt">Font to use for string.</param>
		/// <returns>Smallest Rectangle encompassing the supplied string.</returns>
		public static Rectangle GetGraphicalStringRect(Graphics g, string str, Font fnt)
		{
			Region reg;

			if(string.Empty != str)
			{
				reg = GetGraphicalStringRegion(g, str, fnt);
			}
			else
			{
				reg = new Region(new Rectangle(0, 0, 0, 0));
			}

			RectangleF rectF = reg.GetBounds(g);
			Rectangle rect = new Rectangle((int)Math.Floor(rectF.Left), (int)Math.Floor(rectF.Top), (int)Math.Ceiling(rectF.Width), (int)Math.Ceiling(rectF.Height));

			return rect;
		}

		#endregion Rectangle related Methods



		#region Region related Methods

		/// <summary>
		/// Get the smallest encompassing Region for the Text of the Control supplied using its Text, Font and Graphics context.
		/// </summary>
		/// <param name="ctl">Control to measure Text of.</param>
		/// <returns>Smallest Region encompassing the Text of the supplied Control.</returns>
		public static Region GetGraphicalStringRegion(Control ctl)
		{
			return GetGraphicalStringRegion(ctl.CreateGraphics(), ctl.Text, ctl.Font);
		}

		/// <summary>
		/// Get the smallest encompassing Region for the supplied string, Font and Graphics context.
		/// </summary>
		/// <param name="g">Graphics context object to measure string on.</param>
		/// <param name="str">String to measure.</param>
		/// <param name="fnt">Font to use for string.</param>
		/// <returns>Smallest Region encompassing the supplied string.</returns>
		public static Region GetGraphicalStringRegion(Graphics g, string str, Font fnt)
		{
			StringFormat format = new StringFormat();
			RectangleF rect = new RectangleF(0, 0, 1000, 1000);
			CharacterRange[] ranges = {new CharacterRange(0, str.Length)};
			Region[] regions = new Region[1];

			format.SetMeasurableCharacterRanges(ranges);

			regions = g.MeasureCharacterRanges(str, fnt, rect, format);

			return regions[0];
		}

		#endregion Region related Methods

		#endregion GraphicalString Methods



		#region StringFormattedString Methods

		#region Width related Methods

		/// <summary>
		/// Get the Width of the supplied Control using its Text, Font and Graphics context and the supplied StringFormat.
		/// </summary>
		/// <param name="ctl">Control to measure Text of.</param>
		/// <param name="strfmt">StringFormat to use when measuring the string.</param>
		/// <returns>Width of the Text of the Control supplied using the StringFormat supplied.</returns>
		public static int GetStringFormattedStringWidth(Control ctl, StringFormat strfmt)
		{
			return GetStringFormattedStringRectangle(ctl.CreateGraphics(), ctl.Text, ctl.Font, ctl.ClientRectangle, strfmt).Width;
		}

		/// <summary>
		/// Get the Width of the supplied string using the supplied Font, Rectangle and StringFormat on the supplied Graphics context.
		/// </summary>
		/// <param name="g">Graphics context object to measure string on.</param>
		/// <param name="str">String to measure.</param>
		/// <param name="fnt">Font to use for string.</param>
		/// <param name="rct">Rectangle to measure string in.</param>
		/// <param name="strfmt">StringFormat to use when measuring the string.</param>
		/// <returns>Width of the Text of the Control supplied using the StringFormat supplied.</returns>
		public static int GetStringFormattedStringWidth(Graphics g, string str, Font fnt, Rectangle rct, StringFormat strfmt)
		{
			return GetStringFormattedStringRectangle(g, str, fnt, rct, strfmt).Width;
		}

		#endregion Width related Methods



		#region Rectangle related Methods

		/// <summary>
		/// Get the smallest encompassing Rectangle of the supplied Control using its Text, Font and Graphics context and the supplied StringFormat.
		/// </summary>
		/// <param name="ctl">Control to measure Text of.</param>
		/// <param name="strfmt">StringFormat to use when measuring the string.</param>
		/// <returns>Smallest Rectangle encompassing the Text of the Control supplied using the StringFormat supplied.</returns>
		public static Rectangle GetStringFormattedStringRectangle(Control ctl, StringFormat strfmt)
		{
			return GetStringFormattedStringRectangle(ctl.CreateGraphics(), ctl.Text, ctl.Font, ctl.ClientRectangle, strfmt);
		}

		/// <summary>
		/// Get the smallest encompassing Rectangle of the supplied string using the supplied Font, Rectangle and StringFormat on the supplied Graphics context.
		/// </summary>
		/// <param name="g">Graphics context object to measure string on.</param>
		/// <param name="str">String to measure.</param>
		/// <param name="fnt">Font to use for string.</param>
		/// <param name="rct">Rectangle to measure string in.</param>
		/// <param name="strfmt">StringFormat to use when measuring the string.</param>
		/// <returns>Smallest Rectangle encompassing the Text of the Control supplied using the StringFormat supplied.</returns>
		public static Rectangle GetStringFormattedStringRectangle(Graphics g, string str, Font fnt, Rectangle rct, StringFormat strfmt)
		{
			Region reg;

			if(string.Empty != str)
			{
				reg = GetStringFormattedStringRegion(g, str, fnt, rct, strfmt);
			}
			else
			{
				reg = new Region(new Rectangle(0, 0, 0, 0));
			}

			RectangleF rctF = reg.GetBounds(g);
			Rectangle rctRet = new Rectangle((int)Math.Floor(rctF.Left), (int)Math.Floor(rctF.Top), (int)Math.Ceiling(rctF.Width), (int)Math.Ceiling(rctF.Height));

			return rctRet;
		}

		#endregion Rectangle related Methods



		#region Region related Methods

		/// <summary>
		/// Get the smallest encompassing Region of the supplied Control using its Text, Font and Graphics context and the supplied StringFormat.
		/// </summary>
		/// <param name="ctl">Control to measure Text of.</param>
		/// <param name="strfmt">StringFormat to use when measuring the string.</param>
		/// <returns>Smallest Region encompassing the Text of the Control supplied using the StringFormat supplied.</returns>
		public static Region GetStringFormattedStringRegion(Control ctl, StringFormat strfmt)
		{
			return GetStringFormattedStringRegion(ctl.CreateGraphics(), ctl.Text, ctl.Font, ctl.ClientRectangle, strfmt);
		}

		/// <summary>
		/// Get the smallest encompassing Region of the supplied string using the supplied Font, Rectangle and StringFormat on the supplied Graphics context.
		/// </summary>
		/// <param name="g">Graphics context object to measure string on.</param>
		/// <param name="str">String to measure.</param>
		/// <param name="fnt">Font to use for string.</param>
		/// <param name="rct">Rectangle to measure string in.</param>
		/// <param name="strfmt">StringFormat to use when measuring the string.</param>
		/// <returns>Smallest Region encompassing the Text of the Control supplied using the StringFormat supplied.</returns>
		public static Region GetStringFormattedStringRegion(Graphics g, string str, Font fnt, Rectangle rct, StringFormat strfmt)
		{
			RectangleF rctF = new RectangleF(rct.X, rct.Y, rct.Width, rct.Height);
			CharacterRange[] ranges = {new CharacterRange(0, str.Length)};
			Region[] regions = new Region[1];

			strfmt.SetMeasurableCharacterRanges(ranges);

			regions = g.MeasureCharacterRanges(str, fnt, rctF, strfmt);

			return regions[0];
		}

		#endregion Region related Methods

		#endregion StringFormattedString Methods

	}
}
