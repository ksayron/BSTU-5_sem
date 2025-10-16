using System.Drawing;
using UnityEngine;

public class cameraMovement : MonoBehaviour
{
    public float speed = 3f;
    public Camera camera;
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float horizontalAxisInput = Input.GetAxis("Horizontal");
        if (Input.GetKey(KeyCode.Q))
        {
            camera.fieldOfView += speed * Time.deltaTime; 
            //transform.position += new Vector3(0, 0, speed * Time.deltaTime);
        }
        if (Input.GetKey(KeyCode.E))
        {
            camera.fieldOfView += -speed * Time.deltaTime;
            //transform.position += new Vector3(0, 0, -speed * Time.deltaTime);
        }




    }
}
