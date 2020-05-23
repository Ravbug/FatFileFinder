
#include <wx/dataview.h>
#include "DirectoryData.hpp"

/**
Defines the file-size sort method
*/
class FileSizeModel : public wxDataViewListStore {
#ifndef _WIN32
public:
	/**
	 * The comparator method for the file size displays
	 * @param item1 the first item in the list to compare
	 * @param item2 the second item in the list to compare
	 * @param column unused - this comparator always sorts by client data
	 * ascending - negative = greater or less
	 */
	int Compare(const wxDataViewItem& item1, const wxDataViewItem& item2, unsigned int column, bool ascending) const override {
		//get the client data for use in comparison
		DirectoryData* data1 = (DirectoryData*)GetItemData(item1);
		DirectoryData* data2 = (DirectoryData*)GetItemData(item2);

		int answer = (data1->size > data2->size) ? -1 : 1;

		if (ascending) {
			answer *= -1;
		}

		return answer;
	}
#endif
};
