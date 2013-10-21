using System;
using System.Collections.Generic;
using System.Text;
using Nexus.Reflection;
using Nexus.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace PieChartTest
{
  public class ItemCollectionProxy : ProxyInterface
  {
    public ItemCollectionProxy(PieChart proxy)
    {
      AddProxyObject(proxy);
    }

    [ProxyTypeBinding(typeof(PieChart), "Items")]
    [Browsable(true)]
    [Description("The collection of pie items.")]
    public PieChart.ItemCollection Items
    {
      get
      {
        return ProxyGet<PieChart.ItemCollection>();
      }
      set
      {
        ProxySet(value);
      }
    }

    [ProxyTypeBinding(typeof(PieChart), "Font")]
    [Browsable(true)]
    [Description("The font used with the control.")]
    public Font Font
    {
      get
      {
        return ProxyGet<Font>();
      }
      set
      {
        ProxySet(value);
      }
    }

    [ProxyTypeBinding(typeof(PieChart), "Padding")]
    [Browsable(true)]
    [Description("The padding around the pie")]
    public Padding Padding
    {
      get
      {
        return ProxyGet<Padding>();
      }
      set
      {
        ProxySet(value);
      }
    }
  }
}
