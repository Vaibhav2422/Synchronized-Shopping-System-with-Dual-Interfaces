import { createContext, useContext, useState } from 'react';

const AuthContext = createContext(null);

export function AuthProvider({ children }) {
  const [token, setToken] = useState(() => localStorage.getItem('customer_token'));
  const [customer, setCustomer] = useState(null);

  const signIn = (tok, profile) => {
    localStorage.setItem('customer_token', tok);
    setToken(tok);
    setCustomer(profile);
  };

  const signOut = () => {
    localStorage.removeItem('customer_token');
    setToken(null);
    setCustomer(null);
  };

  return (
    <AuthContext.Provider value={{ token, customer, setCustomer, signIn, signOut }}>
      {children}
    </AuthContext.Provider>
  );
}

export const useAuth = () => useContext(AuthContext);
