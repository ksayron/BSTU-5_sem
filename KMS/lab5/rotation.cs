using UnityEngine;

public class rotation : MonoBehaviour
{
    public float angularSpeedX = 90f;
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        transform.Rotate(angularSpeedX * Time.deltaTime, 0f, 0f,Space.Self);
    }
}
