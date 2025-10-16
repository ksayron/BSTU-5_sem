using UnityEngine;
using UnityEngine.EventSystems;

public class cubePainter : MonoBehaviour,IPointerClickHandler
{
    public bool randomizeColor = true;

    public void OnPointerClick(PointerEventData eventData)
    {
        if (randomizeColor)
        {
            float r = Random.Range(0f, 1f);
            float g = Random.Range(0f, 1f);
            float b = Random.Range(0f, 1f);
            Color col = new Color(r, g, b);

            Renderer rend = GetComponent<Renderer>();
            if (rend != null)
            {
                if (rend.material != null)
                    rend.material.color = col;
            }
        }
        
    }


}
