import { createContext, useContext, useState } from 'react';

const AuthContext = createContext(null);

export function AuthProvider({ children }) {
  const [authed, setAuthed] = useState(() => !!sessionStorage.getItem('admin_authed'));

  const signIn = () => {
    sessionStorage.setItem('admin_authed', '1');
    setAuthed(true);
  };

  const signOut = () => {
    sessionStorage.removeItem('admin_authed');
    setAuthed(false);
  };

  return (
    <AuthContext.Provider value={{ authed, signIn, signOut }}>
      {children}
    </AuthContext.Provider>
  );
}

export const useAuth = () => useContext(AuthContext);
