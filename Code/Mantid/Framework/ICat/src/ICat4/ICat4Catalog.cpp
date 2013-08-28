#include "MantidICat/ICat4/GSoapGenerated/ICat4ICATPortBindingProxy.h"
#include "MantidICat/ICat4/ICat4Catalog.h"
#include "MantidAPI/CatalogFactory.h"
#include "MantidICat/Session.h"
#include "MantidAPI/Progress.h"

namespace Mantid
{
  namespace ICat
  {
    using namespace ICat4;

    DECLARE_CATALOG(ICat4Catalog)

    /// constructor
    ICat4Catalog::ICat4Catalog() { }

    /// destructor
    ICat4Catalog::~ICat4Catalog() { }

    /**
     * Uses ICat API login to connect to catalog.
     * @param username :: login name(E.g. federal id) of the user
     * @param password :: password of the user
     * @param url      :: endpoint url of the catalog
     */
    void ICat4Catalog::login(const std::string& username, const std::string& password, const std::string& url)
    {
      UNUSED_ARG(url)
      ICATPortBindingProxy icat;

      // Define ssl authentication scheme
      setSSLContext(icat);

      // Used to authenticate the user.
      ns1__login login;
      ns1__loginResponse loginResponse;

      // Used to add entries to the login class.
      _ns1__login_credentials_entry entry;

      // Name of the authentication plugin in use.
      std::string plugin("uows");
      login.plugin = &plugin;

      // Making string as cannot convert from const.
      std::string userName(username);
      std::string passWord(password);

      std::string usernameKey("username");
      std::string passwordKey("password");

      // Instantiate an instance of an entry to prevent null pointer.
      // This then allows us to push entries as required below.
      std::vector<_ns1__login_credentials_entry> entries;
      login.credentials.entry = &entries;

      // Setting the username and pass credentials to the login class.
      entry.key   = &usernameKey;
      entry.value = &userName;
      entries.push_back(entry);

      entry.key   = &passwordKey;
      entry.value = &passWord;
      entries.push_back(entry);

      // Is the login successful? (0 if yes, 12 if no).
      int result = icat.login(&login, &loginResponse);

      if (result == 0)
      {
        std::string session_id = *(loginResponse.return_);
        ICat::Session::Instance().setSessionId(session_id);
        ICat::Session::Instance().setUserName(userName);
      }
      else
      {
        throwErrorMessage(icat);
      }
    }

    /**
     * Disconnects the client application from ICat4 based catalog services.
     */
    void ICat4Catalog::logout()
    {
      ICATPortBindingProxy icat;
      setSSLContext(icat);

      ns1__logout request;
      ns1__logoutResponse response;

      std::string sessionID = Session::Instance().getSessionId();
      request.sessionId     = &sessionID;

      int result = icat.logout(&request,&response);

      // If session is set and valid.
      if(result == 0)
      {
        Session::Instance().setSessionId("");
      }
      else
      {
        throw std::runtime_error("You are not currently logged into the system.");
      }
    }

    /**
     * Searches for the relevant data based on user input.
     * @param inputs  :: reference to a class contains search inputs
     * @param ws_sptr :: shared pointer to search results workspace
     */
    void ICat4Catalog::search(const CatalogSearchParam& inputs, Mantid::API::ITableWorkspace_sptr& outputws)
    {
    }

    /**
     * Returns the logged in user's investigations data.
     * @param outputws :: Pointer to table workspace that stores the data.
     */
    void ICat4Catalog::myData(Mantid::API::ITableWorkspace_sptr& outputws)
    {
      ICATPortBindingProxy icat;

      ns1__search request;
      ns1__searchResponse response;

      std::string sessionID = Session::Instance().getSessionId();
      request.sessionId     = &sessionID;

      std::string query = "Investigation INCLUDE Instrument, InvestigationParameter <-> InvestigationUser <-> User[name = :user]";
      request.query     = &query;

      int result = icat.search(&request, &response);

      if (result == 0)
      {
        // Verify data exists in the response.
        if(response.return_.empty())
        {
          throw std::runtime_error("ICat My data search is complete. There are no results to display.");
        }
        else
        {
          // Try to save query result data to workspace.
          saveInvestigations(response.return_, outputws);
        }
      }
      else
      {
        throwErrorMessage(icat);
      }
    }

    /**
     * Saves investigations to a table workspace.
     * @param response :: A vector containing the results of the search query.
     * @param outputws :: Shared pointer to output workspace.
     */
    void ICat4Catalog::saveInvestigations(std::vector<xsd__anyType*> response, API::ITableWorkspace_sptr& outputws)
    {
      // Add rows headers to the output workspace.
      addMyDataColumnHeader(outputws);

      // Add data to each row in the output workspace.
      std::vector<xsd__anyType*>::const_iterator iter;
      for(iter = response.begin(); iter != response.end(); ++iter)
      {
        // Cast from xsd__anyType to subclass (xsd__string).
        ns1__investigation * investigation = dynamic_cast<ns1__investigation*>(*iter);
        // Now attempt to add the relevant data to the output workspace.
        try
        {
          API::TableRow table = outputws->appendRow();
          // Now add the relevant investigation data to the table.
          savetoTableWorkspace(investigation->name, table);    // Investigation ID
          savetoTableWorkspace(investigation->summary, table); // Investigation proposal
          savetoTableWorkspace(investigation->title, table);   // Investigation title
          savetoTableWorkspace(investigation->instrument->name, table); // Instrument name
          savetoTableWorkspace(investigation->parameters[0]->stringValue, table); // Run parameters
        }
        catch(std::runtime_error&)
        {
          throw std::runtime_error("An error occurred when saving the ICat search results data to Workspace");
        }
      }
    }

    /**
     * Adds relevant investigation headers from my data to the output workspace.
     * @param outputws :: shared pointer to table workspace which stores the investigations search result.
     */
    void ICat4Catalog::addMyDataColumnHeader(Mantid::API::ITableWorkspace_sptr& outputws)
    {
      // Add rows to the output workspace...
      outputws->addColumn("str","InvestigationId");
      outputws->addColumn("str","Proposal");
      outputws->addColumn("str","Title");
      outputws->addColumn("str","Instrument");
      outputws->addColumn("str","Run Range");
    }

    /**
     * Returns the datasets associated to the given investigation id.
     * @param investigationId :: unique identifier of the investigation
     * @param datasetsws_sptr :: shared pointer to datasets
     */
    void ICat4Catalog::getDataSets(const long long& investigationId, Mantid::API::ITableWorkspace_sptr& datasetsws_sptr)
    {
    }

    /**
     * Returns the datafiles associated to the given investigation id.
     * @param investigationId  :: unique identifier of the investigation
     * @param datafilesws_sptr :: shared pointer to datasets
     */
    void ICat4Catalog::getDataFiles(const long long& investigationId, Mantid::API::ITableWorkspace_sptr& datafilesws_sptr)
    {
    }

    /**
     * Returns the list of instruments.
     * @param instruments :: instruments list
     */
    void ICat4Catalog::listInstruments(std::vector<std::string>& instruments)
    {
      ICATPortBindingProxy icat;

      ns1__search request;
      ns1__searchResponse response;

      std::string sessionID = Session::Instance().getSessionId();
      request.sessionId     = &sessionID;

      std::string query = "Instrument.name ORDER BY name";
      request.query     = &query;

      int result = icat.search(&request, &response);

      // Was the search successful?
      if (result == 0)
      {
        for(unsigned i = 0; i < response.return_.size(); ++i)
        {
          // Cast from xsd__anyType to subclass (xsd__string).
          xsd__string * instrument = dynamic_cast<xsd__string*>(response.return_[i]);
          // Then add it to the instruments vector.
          instruments.push_back(instrument->__item);
        }
      }
      else
      {
        throwErrorMessage(icat);
      }
    }

    /**
     * Returns the list of investigation types.
     * @param invstTypes :: investigation types list
     */
    void ICat4Catalog::listInvestigationTypes(std::vector<std::string>& invstTypes)
    {
      ICATPortBindingProxy icat;

      ns1__search request;
      ns1__searchResponse response;

      std::string sessionID = Session::Instance().getSessionId();
      request.sessionId     = &sessionID;

      std::string query = "InvestigationType.name ORDER BY name";
      request.query     = &query;

      int result = icat.search(&request, &response);

      // Was the search successful?
      if (result == 0)
      {
        for(unsigned i = 0; i < response.return_.size(); ++i)
        {
          // Cast from xsd__anyType to subclass (xsd__string).
          xsd__string * investigation = dynamic_cast<xsd__string*>(response.return_[i]);
          invstTypes.push_back(investigation->__item);
        }
      }
      else
      {
        throwErrorMessage(icat);
      }
    }

    /**
     * Gets the file location string from the archives.
     * @param fileid       :: id of the file
     * @param filelocation :: location string  of the file
     */
    void ICat4Catalog::getFileLocation(const long long & fileid, std::string & filelocation)
    {
    }

    /**
     * Downloads a file from the given url.
     * @param fileid :: id of the file
     * @param url    :: url  of the file
     */
    void ICat4Catalog::getDownloadURL(const long long & fileid, std::string& url)
    {
    }

    /**
     * Keep the current session alive.
     */
    void ICat4Catalog::keepAlive()
    {
    }

    /**
     * Keep the current session alive in minutes.
     * @return The number of minutes to keep session alive for.
     */
    int ICat4Catalog::keepAliveinminutes()
    {
      return (0);
    }

    /**
     * Defines the SSL authentication scheme.
     * @param icat :: ICATPortBindingProxy object.
     */
    void ICat4Catalog::setSSLContext(ICATPortBindingProxy& icat)
    {
      if (soap_ssl_client_context(&icat,
          SOAP_SSL_NO_AUTHENTICATION, /* use SOAP_SSL_DEFAULT in production code */
          NULL,       /* keyfile: required only when client must authenticate to
              server (see SSL docs on how to obtain this file) */
          NULL,       /* password to read the keyfile */
          NULL,      /* optional cacert file to store trusted certificates */
          NULL,      /* optional capath to directory with trusted certificates */
          NULL      /* if randfile!=NULL: use a file with random data to seed randomness */
      ))
      {
        throwErrorMessage(icat);
      }
    }

    /**
     * Throws an error message (returned by gsoap) to Mantid upper layer.
     * @param icat :: ICATPortBindingProxy object.
     */
    void ICat4Catalog::throwErrorMessage(ICATPortBindingProxy& icat)
    {
      char buf[600];
      const int len = 600;
      icat.soap_sprint_fault(buf,len);
      std::string error(buf);
      std::string begmsg("<message>");
      std::string endmsg("</message>");

      std::basic_string <char>::size_type start = error.find(begmsg);
      std::basic_string <char>::size_type end   = error.find(endmsg);
      std::string exception;

      if(start != std::string::npos && end != std::string::npos)
      {
        exception = error.substr(start + begmsg.length(), end - (start + begmsg.length()) );
      }

      throw std::runtime_error(exception);
    }

  }
}
