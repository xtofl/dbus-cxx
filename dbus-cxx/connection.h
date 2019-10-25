/***************************************************************************
 *   Copyright (C) 2009,2010 by Rick L. Vinyard, Jr.                       *
 *   rvinyard@cs.nmsu.edu                                                  *
 *   Copyright (C) 2019 by Robert Middleton                                *
 *   robert.middleton@rm5248.com                                           *
 *                                                                         *
 *   This file is part of the dbus-cxx library.                            *
 *                                                                         *
 *   The dbus-cxx library is free software; you can redistribute it and/or *
 *   modify it under the terms of the GNU General Public License           *
 *   version 3 as published by the Free Software Foundation.               *
 *                                                                         *
 *   The dbus-cxx library is distributed in the hope that it will be       *
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty   *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 *   General Public License for more details.                              *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this software. If not see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include <list>
#include <deque>
#include <memory>

#include <dbus-cxx/message.h>
#include <dbus-cxx/returnmessage.h>
#include <dbus-cxx/pendingcall.h>
#include <dbus-cxx/watch.h>
#include <dbus-cxx/timeout.h>
#include <dbus-cxx/accumulators.h>
#include <dbus-cxx/object.h>
#include <dbus-cxx/signal_proxy.h>
#include <dbus-cxx/dbus_signal.h>
#include <dbus-cxx/messagefilter.h>
#include <dbus-cxx/methodbase.h>

#include <iostream>

#ifndef DBUSCXX_CONNECTION_H
#define DBUSCXX_CONNECTION_H

namespace DBus
{

  class Object;
  class signal_base;
  class PendingCall;
  class ObjectProxy;

  /**
   * Connection point to the DBus
   *
   * @ingroup core
   *
   * @todo TODO need to deal with signal proxies that change name and/or interface
   *
   * @author Rick L Vinyard Jr <rvinyard@cs.nmsu.edu>
   */
  class Connection
  {
    
    protected:
    
      // TODO dbus_connection_open

      Connection( DBusConnection* cobj = NULL, bool is_private=false );

      /**
       * Connects to a bus daemon and registers the client with it.
       *
       * @param is_private if true a private connection will be created. Otherwise a shared connection is created.
       */
      Connection( BusType type, bool is_private=false );

      Connection( const Connection& other );

    public:
      // TODO dbus_connection_open

      static std::shared_ptr<Connection> create( DBusConnection* cobj = NULL, bool is_private=false );

      /**
       * Connects to a bus daemon and registers the client with it.
       *
       * @param is_private if true a private connection will be created. Otherwise a shared connection is created.
       */
      static std::shared_ptr<Connection> create( BusType type, bool is_private=false );

      static std::shared_ptr<Connection> create( const Connection& other );

      virtual ~Connection();

      /** True if this is a valid connection; false otherwise */
      operator bool() const;

      /** True if this is a valid connection; false otherwise */
      bool is_valid() const;

      /** True if this is a valid connection @e and is private; false otherwise */
      bool is_private() const;

      /** Registers this connection with the bus */
      bool bus_register();

      /** True if this connection is already registered */
      bool is_registered() const;

      /** Gets the unique name of the connection as assigned by the message bus. */
      const char* unique_name() const;

      // TODO set_unique_name() 

      /**
       * The unix user id associated with the name connection or -1 if an
       * error occurred
       */
      unsigned long unix_user( const std::string& name ) const;

      /** The bus' globally unique ID, as described in the D-Bus specification */
      const char* bus_id() const;

      int request_name( const std::string& name, unsigned int flags = 0 );

      int release_name( const std::string& name );

      bool name_has_owner( const std::string& name ) const;

      StartReply start_service( const std::string& name, uint32_t flags ) const;

      bool add_match( const std::string& rule );

      void add_match_nonblocking( const std::string& rule );

      bool remove_match( const std::string& rule );

      void remove_match_nonblocking( const std::string& rule );

      // TODO dbus_connection_close 

      bool is_connected() const;

      bool is_authenticated() const;

      bool is_anonymous() const;

      const char* server_id() const;

      // TODO dbus_connection_preallocate_send
      // TODO dbus_connection_free_preallocated_send
      // TODO dbus_connection_send_preallocated

      uint32_t send( const std::shared_ptr<const Message> );

      /**
       * Sends the message on the connection
       */
      Connection& operator<<( std::shared_ptr<const Message> msg );

      std::shared_ptr<PendingCall> send_with_reply_async( std::shared_ptr<const Message> message, int timeout_milliseconds=-1 ) const;

      std::shared_ptr<const ReturnMessage> send_with_reply_blocking( std::shared_ptr<const Message> msg, int timeout_milliseconds=-1 ) const;

      void flush();

      bool read_write_dispatch( int timeout_milliseconds=-1 );

      bool read_write( int timeout_milliseconds=-1 );

      std::shared_ptr<Message> borrow_message();

      void return_message( std::shared_ptr<Message> message );

      void steal_borrowed_message( std::shared_ptr<Message> message );

      std::shared_ptr<Message> pop_message( );

      DispatchStatus dispatch_status( ) const;

      DispatchStatus dispatch( );

      int unix_fd() const;

      int socket() const;

      unsigned long unix_user() const;

      unsigned long unix_process_id() const;

      // TODO dbus_connection_get_adt_audit_session_data

      // TODO dbus_connection_set_unix_user_function

      // TODO dbus_connection_get_windows_user

      // TODO dbus_connection_set_windows_user_function

      void set_allow_anonymous( bool allow=true );

      void set_route_peer_messages( bool route=true );

      // TODO dbus_connection_try_register_object_path
      // TODO dbus_connection_register_object_path
      // TODO dbus_connection_try_register_fallback
      // TODO dbus_connection_register_fallback
      // TODO dbus_connection_unregister_object_path
      // TODO dbus_connection_get_object_path_data
      // TODO dbus_connection_list_registered

      void set_max_message_size( long size );

      long max_message_size();

      void set_max_received_size( long size );

      long max_received_size();

      long outgoing_size();

      bool has_messages_to_send();

      typedef sigc::signal<bool(std::shared_ptr<Watch>)>::accumulated<InterruptablePredicateAccumulatorDefaultFalse> AddWatchSignal;

      /** Cannot call watch.handle() in a slot connected to this signal */
      AddWatchSignal& signal_add_watch();

      sigc::signal<bool(std::shared_ptr<Watch>)>& signal_remove_watch();

      sigc::signal<void(std::shared_ptr<Watch>)>& signal_watch_toggled();

      typedef sigc::signal<bool(std::shared_ptr<Timeout>)>::accumulated<InterruptablePredicateAccumulatorDefaultFalse> AddTimeoutSignal;
      
      /** Cannot call timeout.handle() in a slot connected to this signal */
      AddTimeoutSignal& signal_add_timeout();

      sigc::signal<bool(std::shared_ptr<Timeout>)>& signal_remove_timeout();

      sigc::signal<bool(std::shared_ptr<Timeout>)>& signal_timeout_toggled();

      sigc::signal<void()>& signal_wakeup_main();

      /** Cannot call dispatch() in a slot connected to this signal */
      sigc::signal<void(DispatchStatus)>& signal_dispatch_status_changed();

      /**
       * Signal emitted during dispatch. A slot returning true will cause the
       * message to be filtered.
       */
      FilterSignal& signal_filter();

      const std::deque<std::shared_ptr<Watch>>& unhandled_watches() const;
      
      void remove_unhandled_watch(const std::shared_ptr<Watch> w);

      std::shared_ptr<Object> create_object( const std::string& path, PrimaryFallback pf=PrimaryFallback::PRIMARY );

      bool register_object( std::shared_ptr<Object> object );

      std::shared_ptr<ObjectPathHandler> create_object( const std::string& path, 
                      sigc::slot<HandlerResult(std::shared_ptr<Connection>, std::shared_ptr<const Message>)>& slot, 
                      PrimaryFallback pf=PrimaryFallback::PRIMARY );

      std::shared_ptr<ObjectPathHandler> create_object( const std::string& path, 
                      HandlerResult (*MessageFunction)(std::shared_ptr<Connection>, std::shared_ptr<const Message>), 
                      PrimaryFallback pf=PrimaryFallback::PRIMARY );

      std::shared_ptr<ObjectProxy> create_object_proxy( const std::string& path );

      std::shared_ptr<ObjectProxy> create_object_proxy( const std::string& destination, const std::string& path );

      bool unregister_object( const std::string& path );

      /**
       * Adds a signal with the given interface and name
       *
       * @return Smart pointer to the newly added signal or a null smart pointer if it couldn't be added
       */
      std::shared_ptr<signal_proxy_base> create_signal_proxy( const std::string& interface, const std::string& name );

      /**
       * Adds a signal with the given path, interface and name
       *
       * @return Smart pointer to the newly added signal or a null smart pointer if it couldn't be added
       */
      std::shared_ptr<signal_proxy_base> create_signal_proxy( const std::string& path, const std::string& interface, const std::string& name );

      template<typename... T_arg>
      std::shared_ptr<signal_proxy<T_arg...> > create_signal_proxy( const std::string& interface, const std::string& name )
      {
        std::shared_ptr<signal_proxy<T_arg...> > sig;
        sig = signal_proxy<T_arg...>::create(interface, name);
        this->add_signal_proxy( sig );
        return sig;
      }
      
      template<typename... T_arg>
      std::shared_ptr<signal_proxy<T_arg...> > create_signal_proxy( const std::string& path, const std::string& interface, const std::string& name )
      {
        std::shared_ptr<signal_proxy<T_arg...> > sig;
        sig = signal_proxy<T_arg...>::create(path, interface, name);
        this->add_signal_proxy( sig );
        return sig;
      }

      /**
       * Adds the given signal proxy to the connection
       */
      std::shared_ptr<signal_proxy_base> add_signal_proxy( std::shared_ptr<signal_proxy_base> signal );

      bool remove_signal_proxy( std::shared_ptr<signal_proxy_base> proxy );

      typedef std::list<std::shared_ptr<signal_proxy_base>> ProxySignals;
      
      typedef std::map<std::string,ProxySignals> NameToProxySignalMap;

      typedef std::map<std::string,NameToProxySignalMap> InterfaceToNameProxySignalMap;

      /** Gets all the signal handlers */
      const InterfaceToNameProxySignalMap& get_signal_proxies();

      /** Gets the signal handlers for a specific interface */
      NameToProxySignalMap get_signal_proxies( const std::string& interface );

      /** Gets the signal handlers for a specific interface and member */
      ProxySignals get_signal_proxies( const std::string& interface, const std::string& member );

      template <class T_return, class... T_arg>
      std::shared_ptr<signal<T_return, T_arg...> > create_signal( const std::string& interface, const std::string& name )
      {
        std::shared_ptr<signal<T_return, T_arg...> > sig;
        sig = signal<T_return, T_arg...>::create(interface, name);
        sig->set_connection(this->self());
        return sig;
      }
      
      template <class T_return, class... T_arg>
      std::shared_ptr<signal<T_return, T_arg...> > create_signal( const std::string& path, const std::string& interface, const std::string& name )
      {
        std::shared_ptr<signal<T_return, T_arg...> > sig;
        sig = signal<T_return, T_arg...>::create(path, interface, name);
        sig->set_connection(this->self());
        return sig;
      }

      //       bool register_object( Object& obj, const std::string & path );
//
//       bool register_signal( signal_base& );

      /** Returns a smart pointer to itself */
      std::shared_ptr<Connection> self();

      /**
       * Given a dbus connection, if it was established through a call to
       * Connection::create() this method will return a copy of the smart
       * pointer returned by the create method.
       *
       * This is accomplished safely because the create() methods store a weak
       * pointer in a dbus data slot associated with the DBusConnection C
       * object, and this method creates the smart pointer from the weak pointer
       * if the weak pointer is still valid.
       *
       * If the DBusConnection was created outside a Connection::create() method
       * or the underlying object has already been deleted this method will
       * return an empty pointer.
       */
      static std::shared_ptr<Connection> self(DBusConnection* c);
      
      DBusConnection* cobj();

      static void set_global_change_sigpipe(bool will_modify_sigpipe=true);

      std::string introspect( const std::string& destination, const std::string& path );

    protected:
      DBusConnection* m_cobj;
      
      bool m_private_flag;
      
      AddWatchSignal m_add_watch_signal;
      
      sigc::signal<bool(std::shared_ptr<Watch>)> m_remove_watch_signal;
      
      sigc::signal<void(std::shared_ptr<Watch>)> m_watch_toggled_signal;
      
      AddTimeoutSignal m_add_timeout_signal;
      
      sigc::signal<bool(std::shared_ptr<Timeout>)> m_remove_timeout_signal;
      
      sigc::signal<bool(std::shared_ptr<Timeout>)> m_timeout_toggled_signal;
      
      sigc::signal<void()> m_wakeup_main_signal;
      
      sigc::signal<void(DispatchStatus)> m_dispatch_status_signal;
      
      FilterSignal m_filter_signal;
      
      std::deque<std::shared_ptr<Watch>> m_unhandled_watches;
      
      typedef std::map<DBusTimeout*,std::shared_ptr<Timeout>> Timeouts;

      Timeouts m_timeouts;

      friend void init(bool);

      static dbus_int32_t m_weak_pointer_slot;

      void initialize( bool is_private );

      std::map<std::string,std::shared_ptr<ObjectPathHandler>> m_created_objects;

      InterfaceToNameProxySignalMap m_proxy_signal_interface_map;

//       std::map<SignalReceiver::pointer, sigc::connection> m_sighandler_iface_conn;

//       std::map<SignalReceiver::pointer, sigc::connection> m_sighandler_member_conn;

//       void on_signal_handler_interface_changed(SignalReceiver::pointer handler);

//       void on_signal_handler_member_changed(SignalReceiver::pointer handler);

//       typedef std::map<std::string,signal_base> SignalVTable;
//
//       typedef std::map<std::string, SignalVTable> InterfaceVTable;

      static dbus_bool_t on_add_watch_callback( DBusWatch* cwatch, void* data );

      static void on_remove_watch_callback( DBusWatch* cwatch, void* data );

      static void on_watch_toggled_callback( DBusWatch* cwatch, void* data );

      static dbus_bool_t on_add_timeout_callback( DBusTimeout* ctimeout, void* data );

      static void on_remove_timeout_callback( DBusTimeout* ctimeout, void* data );

      static void on_timeout_toggled_callback( DBusTimeout* ctimeout, void* data );

      static void on_wakeup_main_callback( void* data );

      static void on_dispatch_status_callback( DBusConnection* connection, DBusDispatchStatus new_status, void* data );

      static DBusHandlerResult on_filter_callback( DBusConnection* connection, DBusMessage* message, void* data );

  };
  
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<DBus::Message> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<const DBus::Message> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<DBus::ReturnMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<const DBus::ReturnMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<DBus::SignalMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

inline
std::shared_ptr<DBus::Connection> operator<<(std::shared_ptr<DBus::Connection> ptr, std::shared_ptr<const DBus::SignalMessage> msg)
{
  if (not ptr) return ptr;
  *ptr << msg;
  return ptr;
}

#endif