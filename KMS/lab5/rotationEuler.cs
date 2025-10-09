using UnityEngine;

public class rotationEuler : MonoBehaviour
{
    public float speedX = 90f;
    public float speedZ = 60f;

    private float angleX;   
    private float angleZ;   
    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        angleX += speedX * Time.deltaTime;
        angleZ += speedZ * Time.deltaTime;

        transform.eulerAngles = new Vector3(angleX,0, angleZ);
    }
}
