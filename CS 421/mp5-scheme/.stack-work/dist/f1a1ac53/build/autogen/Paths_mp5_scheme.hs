{-# LANGUAGE CPP #-}
{-# LANGUAGE NoRebindableSyntax #-}
{-# OPTIONS_GHC -fno-warn-missing-import-lists #-}
{-# OPTIONS_GHC -w #-}
module Paths_mp5_scheme (
    version,
    getBinDir, getLibDir, getDynLibDir, getDataDir, getLibexecDir,
    getDataFileName, getSysconfDir
  ) where


import qualified Control.Exception as Exception
import qualified Data.List as List
import Data.Version (Version(..))
import System.Environment (getEnv)
import Prelude


#if defined(VERSION_base)

#if MIN_VERSION_base(4,0,0)
catchIO :: IO a -> (Exception.IOException -> IO a) -> IO a
#else
catchIO :: IO a -> (Exception.Exception -> IO a) -> IO a
#endif

#else
catchIO :: IO a -> (Exception.IOException -> IO a) -> IO a
#endif
catchIO = Exception.catch

version :: Version
version = Version [0,2,0,0] []

getDataFileName :: FilePath -> IO FilePath
getDataFileName name = do
  dir <- getDataDir
  return (dir `joinFileName` name)

getBinDir, getLibDir, getDynLibDir, getDataDir, getLibexecDir, getSysconfDir :: IO FilePath



bindir, libdir, dynlibdir, datadir, libexecdir, sysconfdir :: FilePath
bindir     = "C:\\Users\\yflin\\OneDrive\\Documents\\UIUC\\2025- Semester 2\\CS 421\\mp5-scheme\\.stack-work\\install\\647947d9\\bin"
libdir     = "C:\\Users\\yflin\\OneDrive\\Documents\\UIUC\\2025- Semester 2\\CS 421\\mp5-scheme\\.stack-work\\install\\647947d9\\lib\\x86_64-windows-ghc-9.4.8\\mp5-scheme-0.2.0.0-1J52y9HBdKJGs2u1gUdwYv"
dynlibdir  = "C:\\Users\\yflin\\OneDrive\\Documents\\UIUC\\2025- Semester 2\\CS 421\\mp5-scheme\\.stack-work\\install\\647947d9\\lib\\x86_64-windows-ghc-9.4.8"
datadir    = "C:\\Users\\yflin\\OneDrive\\Documents\\UIUC\\2025- Semester 2\\CS 421\\mp5-scheme\\.stack-work\\install\\647947d9\\share\\x86_64-windows-ghc-9.4.8\\mp5-scheme-0.2.0.0"
libexecdir = "C:\\Users\\yflin\\OneDrive\\Documents\\UIUC\\2025- Semester 2\\CS 421\\mp5-scheme\\.stack-work\\install\\647947d9\\libexec\\x86_64-windows-ghc-9.4.8\\mp5-scheme-0.2.0.0"
sysconfdir = "C:\\Users\\yflin\\OneDrive\\Documents\\UIUC\\2025- Semester 2\\CS 421\\mp5-scheme\\.stack-work\\install\\647947d9\\etc"

getBinDir     = catchIO (getEnv "mp5_scheme_bindir")     (\_ -> return bindir)
getLibDir     = catchIO (getEnv "mp5_scheme_libdir")     (\_ -> return libdir)
getDynLibDir  = catchIO (getEnv "mp5_scheme_dynlibdir")  (\_ -> return dynlibdir)
getDataDir    = catchIO (getEnv "mp5_scheme_datadir")    (\_ -> return datadir)
getLibexecDir = catchIO (getEnv "mp5_scheme_libexecdir") (\_ -> return libexecdir)
getSysconfDir = catchIO (getEnv "mp5_scheme_sysconfdir") (\_ -> return sysconfdir)




joinFileName :: String -> String -> FilePath
joinFileName ""  fname = fname
joinFileName "." fname = fname
joinFileName dir ""    = dir
joinFileName dir fname
  | isPathSeparator (List.last dir) = dir ++ fname
  | otherwise                       = dir ++ pathSeparator : fname

pathSeparator :: Char
pathSeparator = '\\'

isPathSeparator :: Char -> Bool
isPathSeparator c = c == '/' || c == '\\'
