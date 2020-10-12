#ifndef BEE_FISH_DATABASE__PAGE_CACHE
#define BEE_FISH_DATABASE__PAGE_CACHE
#include <map>
#include <list>
#include "page.h"

namespace bee::fish::database {

   class Database;
   
   struct CachedPage
   {
      Page* _page;
      File::Size _pageIndex;
      bool _isDirty;
         
      CachedPage(File::Size pageIndex = 0)
      {
         _page = (Page*)malloc(PAGE_SIZE);
         _pageIndex = pageIndex;
         _isDirty = false;
      }
         
      ~CachedPage()
      {
         free(_page);
      }
      
   };
   
   class PageCache :
      public map<File::Size, CachedPage*>
   {
   protected:
      list<File::Size> _pageList;
      Database* _database;
   public:
      PageCache(Database* database) :
         _database(database)
      {
      }
      
      CachedPage* operator []
      (File::Size pageIndex);
      
      ~PageCache();
      

   };
}

#include "database.h"

namespace bee::fish::database {

   inline PageCache::~PageCache()
   {
      for (auto pair : (*this))
      {
         CachedPage* cachedPage = pair.second;
            
         if (cachedPage->_isDirty)
         {
            _database->writePage(
               cachedPage->_pageIndex,
               cachedPage->_page
            );
         }
            
         delete cachedPage;
      }
   }
   
   inline CachedPage* PageCache:: operator []
   (File::Size pageIndex)
   {
      if (count(pageIndex) == 0)
      {
#ifdef DEBUG
         cerr << "{Page Miss}";
#endif
         // Construct the new page cache
         CachedPage* cachedPage =
            new CachedPage(pageIndex);

         // Read the page from the database file
         _database->readPage(pageIndex, cachedPage->_page);
            
         // Set the page cache in the pages map
         emplace(pageIndex, cachedPage);
         
         // Keep a list of older to newer pages
         _pageList.push_back(pageIndex);
            
         // Delete older pages
         while ( _pageList.size() >
                 _database->_maxPageCache )
         {
            // Get the oldest page index
            File::Size oldPageIndex =
               _pageList.front();
                  
            // Remove the oldest page index
            _pageList.pop_front();
               
            // Get the corresponding cache page
            CachedPage* cachedPage =
               at(oldPageIndex);
                  
            // Write the page if its been changrd
            if (cachedPage->_isDirty)
               _database->writePage(
                  cachedPage->_pageIndex,
                  cachedPage->_page
               );
                  
            // Remove the cached page from tbe map
            erase(oldPageIndex);
               
            delete cachedPage;
               
         }
      }
#ifdef DEBUG
      else
      {
         cerr << "{Page Hit}";
         // need to shuffle the list
      }
#endif

      return at(pageIndex);
   }
   
}
#endif