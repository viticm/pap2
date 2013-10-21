/// Author:  Matthew Johnson
/// Version: 1.0
/// Date:    March 13, 2006
/// Notice:  You are free to use this code as you wish.  There are no guarantees whatsoever about
/// its usability or fitness of purpose.

#region using
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;
#endregion

namespace Nexus.Reflection
{
  /// <summary>
  /// Provides a base for implementers who want their properties and methods to be bound to one or more
  /// other classes.
  /// </summary>
  /// <remarks>
  /// Implementers of this class should call <see cref="ProxySet"/>, <see cref="ProxyGet"/>, and <see cref="ProxyInvoke"/>
  /// to have the ProxyInterface automatically call properties and methods on objects that are in list
  /// of proxy objects.  By default, the ProxyInterface will attempt to call a method with the same name
  /// and parameters as the original method.  However, you can bind each proxy call to different methods for different
  /// types.  For each type that this class will proxy, you can add a <see cref="ProxyTypeBindingAttribute"/> to
  /// a property or method that specifies with member of the proxied type should be accessed.
  /// </remarks>
  public abstract class ProxyInterface
  {
    #region Fields
    private List<object> objects = new List<object>();
    private Dictionary<MethodBase, ProxyAttributeCollection> attributes = new Dictionary<MethodBase, ProxyAttributeCollection>();
    #endregion

    #region Methods
    /// <summary>
    /// Performs the get operation on the first object in the list of proxy objects (the default object).
    /// </summary>
    /// <typeparam name="T">The type to cast the return value as.</typeparam>
    /// <param name="args">The indices to pass to the get method.</param>
    /// <returns>The result of executing a get operation on the default proxy object, or the default value of T
    /// if there are no proxy objects.
    /// </returns>
    public T ProxyGet<T>(params object[] args)
    {
      if (objects.Count == 0)
      {
        return default(T);
      }

      StackTrace trace = new StackTrace();
      StackFrame frame = trace.GetFrame(1);
      MethodInfo method = frame.GetMethod() as MethodInfo;
      ProxyAttributeCollection attributes = GetAttributes(method);
      ProxyTypeBindingAttribute binding = attributes.GetTypeBinding(objects[0]);
      MemberInfoAccessor accessor = binding.GetTargetAccessor(typeof(T), args);
      return (T)accessor.Get(objects[0], args);
    }

    /// <summary>
    /// Performs the set operation on all proxy objects.
    /// </summary>
    /// <param name="value">The value to set on the proxy objects.</param>
    /// <param name="args">The indices of the set operation.</param>
    protected void ProxySet(object value, params object[] args)
    {
      if (objects.Count == 0)
      {
        return;
      }

      StackTrace trace = new StackTrace();
      StackFrame frame = trace.GetFrame(1);
      MethodInfo method = frame.GetMethod() as MethodInfo;
      ProxyAttributeCollection attributes = GetAttributes(method);
      foreach (object invoke in objects)
      {
        ProxyTypeBindingAttribute binding = attributes.GetTypeBinding(invoke);
        object[] argsWithValue = new object[args.Length + 1];
        argsWithValue[0] = value;
        Array.Copy(args, 0, argsWithValue, 1, args.Length);

        MemberInfoAccessor accessor = binding.GetTargetAccessor(typeof(void), argsWithValue);
        accessor.Set(invoke, value, args);
      }
    }

    /// <summary>
    /// Invokes proxy methods which do not return a value.
    /// </summary>
    /// <param name="args">The arguments to pass to the method.</param>
    protected void ProxyInvoke(params object[] args)
    {
      if (objects.Count == 0)
      {
        return;
      }

      StackTrace trace = new StackTrace();
      StackFrame frame = trace.GetFrame(1);
      MethodInfo method = frame.GetMethod() as MethodInfo;
      ProxyAttributeCollection attributes = GetAttributes(method);

      foreach (object invoke in objects)
      {
        ProxyTypeBindingAttribute binding = attributes.GetTypeBinding(invoke);
        MemberInfoAccessor accessor = binding.GetTargetAccessor(typeof(void), args);
        accessor.Get(invoke, args);
      }
    }

    /// <summary>
    /// Invokes proxy methods which return a value.
    /// </summary>
    /// <typeparam name="T">The return type of the method being invoked.</typeparam>
    /// <param name="args">The arguments to pass to the method.</param>
    /// <returns>The return value of the default proxy method.</returns>
    /// <remarks>This method will invoke methods on all of the proxy objects, but will return
    /// the result of the invocation on the default object.
    /// </remarks>
    protected T ProxyInvoke<T>(params object[] args)
    {
      if (objects.Count == 0)
      {
        return default(T);
      }

      StackTrace trace = new StackTrace();
      StackFrame frame = trace.GetFrame(1);
      MethodInfo method = frame.GetMethod() as MethodInfo;
      ProxyAttributeCollection attributes = GetAttributes(method);

      bool first = true;
      T result = default(T);
      foreach (object invoke in objects)
      {
        ProxyTypeBindingAttribute binding = attributes.GetTypeBinding(invoke);
        MemberInfoAccessor accessor = binding.GetTargetAccessor(typeof(T), args);
        if (first)
        {
          result = (T)accessor.Get(invoke, args);
          first = false;
        }
        else
        {
          accessor.Get(invoke, args);
        }
      }

      return result;
    }

    /// <summary>
    /// Adds an object to the list of proxy objects.
    /// </summary>
    /// <param name="proxy">The object to proxy.</param>
    protected void AddProxyObject(object proxy)
    {
      objects.Add(proxy);
    }

    /// <summary>
    /// Adds an object to the list of proxy objects and makes it the default object.
    /// </summary>
    /// <param name="proxy"></param>
    protected void AddDefaultProxyObject(object proxy)
    {
      objects.Insert(0, proxy);
    }

    /// <summary>
    /// Removes an object from the list of proxy objects.
    /// </summary>
    /// <param name="proxy">The object to remove from the list.</param>
    protected void RemoveProxyObject(object proxy)
    {
      objects.Remove(proxy);
    }

    /// <summary>
    /// Gets the <see cref="ProxyAttributeCollection"/> for a given <see cref="MethodInfo"/>.
    /// </summary>
    /// <param name="method">The method to get the proxy attributes for.</param>
    /// <returns>The collection of attributes for the given method as
    /// revealed by reflection of the method's attributes.
    /// </returns>
    private ProxyAttributeCollection GetAttributes(MethodInfo method)
    {
      if (!attributes.ContainsKey(method))
      {
        ProxyAttributeCollection collection = ReflectMethod(method);
        attributes.Add(method, collection);
      }

      return attributes[method];
    }

    /// <summary>
    /// Performs reflection on a method in order to determine which methods on proxy objects
    /// the method stub is bound to.
    /// </summary>
    /// <param name="method">The method to reflect over.</param>
    /// <returns>The collection of attributes about the method.</returns>
    private ProxyAttributeCollection ReflectMethod(MethodInfo method)
    {
      ProxyAttributeCollection collection = new ProxyAttributeCollection(method);

      // get the information that is applied directly to the method
      object[] typeBindings = method.GetCustomAttributes(typeof(ProxyTypeBindingAttribute), true);
      foreach (ProxyTypeBindingAttribute attribute in typeBindings)
      {
        collection.AddTypeBinding(attribute);
      }

      // if the method is a get or set method on a property, also get the attributes applied to the property
      if (MemberInfoAccessor.IsGetMethod(method) || MemberInfoAccessor.IsSetMethod(method))
      {
        PropertyInfo propertyInfo = MemberInfoAccessor.GetProperty(method);
        object[] moreBindings = propertyInfo.GetCustomAttributes(typeof(ProxyTypeBindingAttribute), true);
        foreach (ProxyTypeBindingAttribute attribute in moreBindings)
        {
          collection.AddTypeBinding(attribute);
        }
      }

      return collection;
    }
    #endregion
  }
}