//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10
//      File_base        : HtmlBuilder
//      File_ext         : cs
//      Author           : 徐晓刚
//      Description      : HTML片断创建器
//
//////////////////////////////////////////////////////////////////////

using System;
using System.Text;
using System.Web.UI.WebControls;

namespace FSEye.WebControls
{
	/// <summary>
	/// HTML片断创建器
	/// </summary>
	public class HtmlBuilder
	{
		public HtmlBuilder()
		{
		}

		public static string CreateProgressBar(float progress, Orientation orientation, Unit width, Unit height, Unit borderWidth)
		{
			return CreateProgressBar(
				progress,
				orientation,
				width,
				height,
				borderWidth,
				string.Empty,
				string.Empty,
				string.Empty,
				string.Empty,
				string.Empty,
				string.Empty,
				string.Empty,
				string.Empty
				);
		}

		public static string CreateProgressBar(
			float progress,
			Orientation orientation,
			Unit width,
			Unit height,
			Unit borderWidth,			
			string borderCssClass, 
			string borderStyle,
			string processedCssClass,
			string processedStyle,
			string unprocessedCssClass,
			string unprocessedStyle,
			string completeCssClass,
			string completeStyle
			)
		{
			if (progress >= 0 && progress <= 1)
			{
				StringBuilder html = new StringBuilder();
				int percentage = (int)(progress * 100);

				string borderCssClassStr = string.Empty;
				if (borderCssClass != null && borderCssClass.Length > 0)
				{
					borderCssClassStr = String.Format(" class='{0}'", borderCssClass);
				}

				string borderStyleStr = string.Empty;
				if (borderStyle != null && borderStyle.Length > 0)
				{
					borderStyleStr = String.Format(" style='{0}'", borderStyle);
				}

				string processedCssClassStr = string.Empty;
				if (processedCssClass != null && processedCssClass.Length > 0)
				{
					processedCssClassStr = String.Format(" class='{0}'", processedCssClass);
				}

				string unprocessedCssClassStr = string.Empty;
				if (unprocessedCssClass != null && unprocessedCssClass.Length > 0)
				{
					unprocessedCssClassStr = String.Format(" class='{0}'", unprocessedCssClass);
				}

				string completeCssClassStr = string.Empty;
				if (completeCssClass != null && completeCssClass.Length > 0)
				{
					completeCssClassStr = String.Format(" class='{0}'", completeCssClass);
				}

				string processedStyleStr = string.Empty;
				if (processedStyle != null && processedStyle.Length > 0)
				{
					processedStyleStr = String.Format(" style='{0}'", processedStyle);
				}

				string unprocessedStyleStr = string.Empty;
				if (unprocessedStyle != null && unprocessedStyle.Length > 0)
				{
					unprocessedStyleStr = String.Format(" style='{0}'", unprocessedStyle);
				}

				string completeStyleStr = string.Empty;
				if (completeStyle != null && completeStyle.Length > 0)
				{
					completeStyleStr = String.Format(" style='{0}'", completeStyle);
				}

				html.AppendFormat("<table width='{0}' height='{1}' cellPadding='0' cellSpacing='0'{2}{3}>", width, height, borderCssClassStr, borderStyleStr);

				if (orientation == Orientation.Horizontal)
				{
					html.Append("<tr>");
					if (percentage <= 0)
					{
						html.Append("<td></td>");
					}
					else if (percentage < 100)
					{
						html.Append("<td bgcolor='");
						html.Append("#FF0000");
						html.Append("' width='");
						html.Append(percentage);
						html.Append("%'></td><td></td>");
					}
					else
					{
						html.Append("<td bgcolor='");
						html.Append("#00FF00");
						html.Append("'></td>");
					}
					html.Append("</tr>");

					//if (percentage <= 0)
					//{
					//    html.AppendFormat("<tr><td></td></tr>");
					//}
					//else if (percentage < 100)
					//{
					//    html.AppendFormat("<tr><td align='left'><table cellPadding='0' cellSpacing='0' width='{0}' height='100%'{1}{2}><tr><td></td></tr></table></td></tr>", new Unit(width.Value * progress, width.Type), processedStyleStr, processedCssClassStr);
					//}
					//else
					//{
					//    html.AppendFormat("<tr><td align='left'><table cellPadding='0' cellSpacing='0' width='{0}' height='100%'{1}{2}><tr><td></td></tr></table></td></tr>", new Unit(width.Value * progress, width.Type), completeStyleStr, completeCssClassStr);
					//}
				}
				else if (orientation == Orientation.Vertical)
				{
					if (percentage <= 0)
					{
						html.Append("<tr><td></td></tr>");
					}
					else if (percentage < 100)
					{
						html.AppendFormat("<tr><td valign='bottom'><table cellPadding='0' cellSpacing='0' width='100%' height='{0}'{1}{2}><tr><td></td></tr></table></td></tr>", new Unit(height.Value * progress, UnitType.Pixel), processedStyleStr, processedCssClassStr);
					}
					else
					{
						html.AppendFormat("<tr><td><table cellPadding='0' cellSpacing='0' width='100%' height='{0}'{1}{2}><tr><td></td></tr></table></td></tr>", new Unit(height.Value * progress, UnitType.Pixel), completeStyleStr, completeCssClassStr);
					}
				}

				html.Append("</table>");

				return html.ToString();
			}
			else
			{
				return string.Empty;
			}
		}

		public static string CreateImage(string imgPath, int width, int height)
		{
			if (imgPath != null && imgPath.Length > 0 && width > 0 && height > 0)
			{
				StringBuilder html = new StringBuilder();
				html.Append("<img width='");
				html.Append(width);
				html.Append("' height='");
				html.Append(height);
				html.Append("' src='");
				html.Append(imgPath);
				html.Append("' />");

				return html.ToString();
			}
			else
			{
				return string.Empty;
			}
		}

		public static string CreateHidden(string name, string value)
		{
			if (name != null && name.Length > 0 && value != null && value.Length > 0)
			{
				StringBuilder html = new StringBuilder();
				html.Append("<input type='hidden' id='");
				html.Append(name);
				html.Append("' value='");
				html.Append(value);
				html.Append("' />");

				return html.ToString();
			}
			else
			{
				return string.Empty;
			}
		}

		public static string CreateCheckbox(string name, string label, bool isChecked, bool enabled, string onClick)
		{
			StringBuilder html = new StringBuilder();

			html.Append("<input type='checkbox'");

			if (name != null)
			{
				html.Append(" id='");
				html.Append(name);
				html.Append("'");
			}

			if (isChecked)
			{
				html.Append(" checked");
			}

			if (!enabled)
			{
				html.Append(" disabled");
			}

			if (onClick != null)
			{
				html.Append(" onclick=\"" + onClick + "\"");
			}

			html.Append(">");

			if (label != null)
			{
				html.Append(label);
			}

			html.Append("</input>");

			return html.ToString();
		}

		public static string CreateSelect(string name, string[] options, int selectedIndex, bool enabled)
		{
			StringBuilder html = new StringBuilder();

			html.Append("<select");
			if (name != null)
			{
				html.Append(" id='");
				html.Append(name);
				html.Append("'");
			}
			if (!enabled)
			{
				html.Append(" disabled");
			}
			html.Append(">");

			if (options != null)
			{
				if (selectedIndex < 0 || selectedIndex >= options.Length)
				{
					throw new IndexOutOfRangeException("selectedIndex=" + selectedIndex);
				}

				for (int i = 0; i < options.Length; i++)
				{
					html.Append("<option value='");
					html.Append(options[i]);
					html.Append("' ");
					if (selectedIndex == i)
					{
						html.Append("selected");
					}
					html.Append(">");
					html.Append(options[i]);
					html.Append("</option>");
				}
			}

			html.Append("</select>");

			return html.ToString();
		}

		/// <summary>
		/// 创建按钮
		/// </summary>
		/// <param name="name">按钮ID</param>
		/// <param name="text">按钮文字</param>
		/// <param name="enabled">是否启用</param>
		/// <param name="onClick">点击事件</param>
		public static string CreateButton(string name, string text, bool enabled, string onClick)
		{
			StringBuilder html = new StringBuilder();

			html.Append("<input type=\"button\"");

			if (name != null)
			{
				html.Append(" id=\"");
				html.Append(name);
				html.Append("\"");
			}

			if (text != null)
			{
				html.Append(" value=\"");
				html.Append(text);
				html.Append("\"");
			}

			if (!enabled)
			{
				html.Append(" disabled");
			}

			if (onClick != null)
			{
				html.Append(" onclick=\"");
				html.Append(onClick);
				html.Append("\"");
			}

			html.Append(" />");

			return html.ToString();
		}
	}
}