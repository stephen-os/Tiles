#include "TextureSelection.h"

namespace Tiles
{

	void TextureSelection::Select(int texture)
	{
		if (!m_MultiSelect)
		{
			Clear(); 
		}

		m_SelectedTextures.insert(texture);
	}
}