<?xml version="1.0" encoding="utf-8" ?>
<%@ page contentType="text/xml;charset=UTF-8"
         session="false"
         import="java.util.Vector,
                 org.w3c.dom.Element,
                 org.apache.juddi.util.xml.XMLUtils,
                 org.apache.juddi.handler.HandlerMaker,
                 org.apache.juddi.handler.IHandler,
                 org.apache.juddi.datatype.RegistryObject,
                 org.apache.juddi.datatype.business.BusinessEntity,
                 org.apache.juddi.datatype.response.BusinessDetail,
                 org.apache.juddi.datatype.service.BusinessService,
                 org.apache.juddi.datatype.response.ServiceDetail,
                 org.apache.juddi.datatype.tmodel.TModel,
                 org.apache.juddi.datatype.response.TModelDetail,
                 org.apache.juddi.datatype.binding.BindingTemplate,
                 org.apache.juddi.datatype.response.BindingDetail,
                 org.apache.juddi.registry.RegistryServlet,
                 org.apache.juddi.registry.RegistryEngine,
                 org.apache.juddi.error.RegistryException"               
%>
<%!
    
    BusinessEntity getBusinessEntity(String key)
    	throws RegistryException
    {
      BusinessEntity business = null;
      
      RegistryEngine registry = RegistryServlet.getRegistry();
      BusinessDetail detail = registry.getBusinessDetail(key);
      if (detail != null)
      {
    		Vector collection = detail.getBusinessEntityVector();
    		if ((collection != null) && (collection.size() > 0))
    			business = (BusinessEntity)collection.elementAt(0);
      }
    	
      return business;
    }
    
    BusinessService getBusinessService(String key)
    	throws RegistryException
    {
      BusinessService service = null;
      
    	RegistryEngine registry = RegistryServlet.getRegistry();
    	ServiceDetail detail = registry.getServiceDetail(key);
    	if (detail != null)
    	{
    		Vector collection = detail.getBusinessServiceVector();
    		if ((collection != null) && (collection.size() > 0))
    			service = (BusinessService)collection.elementAt(0);
    	}
    	
    	return service;
    }

    BindingTemplate getBindingTemplate(String key)
    	throws RegistryException
    {
      BindingTemplate binding = null;
      
    	RegistryEngine registry = RegistryServlet.getRegistry();
    	BindingDetail detail = registry.getBindingDetail(key);
    	if (detail != null)
    	{
    		Vector collection = detail.getBindingTemplateVector();
    		if ((collection != null) && (collection.size() > 0))
    			binding = (BindingTemplate)collection.elementAt(0);
    	}
    	
    	return binding;
    }

    TModel getTModel(String key)
    	throws RegistryException
    {
      TModel tModel = null;
      
    	RegistryEngine registry = RegistryServlet.getRegistry();
    	TModelDetail detail = registry.getTModelDetail(key);
    	if (detail != null)
    	{
    		Vector collection = detail.getTModelVector();
    		if ((collection != null) && (collection.size() > 0))
    			tModel = (TModel)collection.elementAt(0);
    	}
    	
    	return tModel;
    }
    
    Element getElement(RegistryObject object)
    {
      Element element = XMLUtils.newRootElement();
      
      HandlerMaker maker = HandlerMaker.getInstance();
      IHandler handler = maker.lookup(object.getClass().getName().toLowerCase());
      handler.marshal(object,element);

      return (Element)element.getFirstChild();
    }
      
%>
<%

    try
    {
	    String businessKey = request.getParameter("businesskey");
	    if (businessKey != null)
	    {
	      BusinessEntity object = getBusinessEntity(businessKey);
	      if (object == null)
	      	throw new RegistryException("The Business Key specified " +
	      		"did not match with any known Business Entities.");
	
	      Element element = getElement(object);
	      XMLUtils.writeXML(element,out);
	    }
	    	
	    String serviceKey = request.getParameter("servicekey");
	    if (serviceKey != null)
	    {
	      BusinessService object = getBusinessService(serviceKey);
	      if (object == null)
	      	throw new RegistryException("The Service Key specified " +
	      		"did not match with any known Business Services.");
	      	
	      Element element = getElement(object);
	      XMLUtils.writeXML(element,out);
	    }
	
	    String bindingKey = request.getParameter("bindingkey");
	    if (bindingKey != null)
	    {
	      BindingTemplate object = getBindingTemplate(bindingKey);
	      if (object == null)
	      	throw new RegistryException("The Binding Template Key " +
	      		"specified did not match with any known Binding Templates.");
	
	      Element element = getElement(object);
	      XMLUtils.writeXML(element,out);
	    }
		
	    String tModelKey = request.getParameter("tmodelkey");
	    if (tModelKey != null)
	    {
	      TModel object = getTModel(tModelKey);
	      if (object == null)
	      	throw new RegistryException("The TModel Key specified " +
	      		"did not match with any known TModels.");
	
	      Element element = getElement(object);
	      XMLUtils.writeXML(element,out);
	    }
		}
		catch(RegistryException regex)
		{
			//out.println();	
		}
		catch(Exception regex)
		{
			//out.println();	
		}
%>
